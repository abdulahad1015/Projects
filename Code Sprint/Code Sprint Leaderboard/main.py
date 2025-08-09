# pyinstaller --onefile --windowed --hidden-import=pygame --add-data "car.gif:." --add-data "droad.png:." --add-data "calibri.ttf:." --add-data "titillium.ttf:." --add-data "alpha.ttf:."  main.py

import pygame, random, sys, requests, time, threading, os
from result import get_participants

# List of contest numbers in the order provided.
CONTEST_LIST = [694280, 694489, 694187, 694188, 694189, 694191]
CONTEST_NO = None  # Will be determined by select_current_contest()
critical = True

# Global variables for contest timer (all in seconds)
contest_begin = 0    # Unix timestamp when contest begins
contest_length = 0   # Duration in seconds
contest_end = 0      # contest_begin + contest_length

# Global variables for ranking and arrows
old_ranking = []
previous_top_n_names = []  # Stores the previous top N driver names
arrow_states = {}  # participant_name -> arrow symbol ('↑'/'↓'/'')

car_names = []           # List of participant names (current ranking order)
participants_sorted = [] # List of tuples: (name, score, penalty)

# Global list for car objects (only for top N participants)
cars = []
MAX_CARS = 5  # Only top 5 participants get cars

# A flag to indicate new data has arrived so we update target positions
data_updated = False

def resource_path(relative_path):
    """Helper to handle PyInstaller's resource paths."""
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")
    return os.path.join(base_path, relative_path)

def get_time(contest_no):
    """
    Fetch contest timing data from VJudge.
    Returns a tuple (begin, length), where:
      - 'begin' is the contest start time as a Unix timestamp (in seconds)
      - 'length' is the contest duration in seconds.
    VJudge returns these values in milliseconds, so we convert them.
    """
    response = requests.get(f"https://vjudge.net/contest/rank/single/{contest_no}")
    vjudge = response.json()
    begin = vjudge['begin']
    length = vjudge['length']
    # Convert milliseconds to seconds
    begin = int(begin / 1000)
    length = int(length / 1000)
    return begin, length

def select_current_contest():
    """
    Loop through your contest numbers in order.
    For each contest, fetch its timing data.
    If the current time is less than (begin + length) (i.e. the contest is ongoing or hasn't ended yet),
    return that contest number.
    """
    now = int(time.time())
    for contest in CONTEST_LIST:
        try:
            begin, length = get_time(contest)
            print(begin, length)
        except Exception:
            continue
        if now < (begin + length):
            return contest
    return None

# Select the current contest before starting.
current_contest = select_current_contest()
if current_contest is None:
    print("No ongoing or upcoming contest found. Exiting.")
    sys.exit(0)
else:
    CONTEST_NO = current_contest
    contest_begin, contest_length = get_time(CONTEST_NO)
    contest_end = contest_begin + contest_length
    print("Using contest number:", CONTEST_NO)

def update_contest_timer():
    """
    Every minute, fetch the contest's timing data to see if its begin time or length has changed.
    This updates the global contest_begin, contest_length, and contest_end variables.
    """
    global contest_begin, contest_length, contest_end
    while True:
        try:
            new_begin, new_length = get_time(CONTEST_NO)
            contest_begin = new_begin
            contest_length = new_length
            contest_end = contest_begin + contest_length
        except Exception as e:
            print("Error updating contest timer:", e)
        time.sleep(60)

def get_result():
    """
    Background thread function that:
      - Fetches participant data
      - Sorts them by score/penalty
      - Determines arrow states (↑/↓/steady) based on rank changes
      - Updates global data and sets a flag to update car target positions.
    """
    global car_names, participants_sorted, critical, data_updated
    global old_ranking, arrow_states

    while True:
        participants = get_participants(CONTEST_NO)
        lst = []
        for p in participants.values():
            lst.append((p['name'], p['score'], p['penalty']))
        # Sort participants by score and penalty descending
        lst = sorted(lst, key=lambda i: (i[1], -i[2]), reverse=True)
        # Append some dummy participants for demonstration if needed.
        if len(lst) < 15:
            lst += [('Areeb', 0, 0), ('Abrar', 0, 0), ('404 brain not found', 0, 0)]
        # lst += [('Areeb', 0, 0), ('Abrar', 0, 0), ('404 brain not found', 0, 0)]
        new_ranking = [p[0] for p in lst]

        # Update arrow states by comparing new_ranking to old_ranking
        for new_index, (name, score, penalty) in enumerate(lst):
            if name in old_ranking:
                old_index = old_ranking.index(name)
                if new_index < old_index:
                    arrow_states[name] = '↑'
                elif new_index > old_index:
                    arrow_states[name] = '↓'
                else:
                    arrow_states[name] = ''
            else:
                arrow_states[name] = ''
        old_ranking = new_ranking.copy()

        # Update global data for the main thread
        critical = False
        participants_sorted = lst
        car_names = new_ranking
        critical = True

        # Set flag to update top cars (their target positions)
        data_updated = True

        time.sleep(5)

def update_top_cars():
    """
    Update (or create) the car objects for the top MAX_CARS participants.
    Instead of immediately changing positions, update each car's target_x (and target_y)
    based on its new rank.
    
    When a new driver enters the top N, spawn it at the y-position of the car it replaced
    (if available), so that it appears in the same lane.
    """
    global cars, participants_sorted, previous_top_n_names

    # Identify the top N participants (by sorted data)
    top_n = participants_sorted[:MAX_CARS]
    top_n_names = [p[0] for p in top_n]
    top_n_set = set(top_n_names)

    # Remove cars for drivers no longer in top N
    cars[:] = [c for c in cars if c["name"] in top_n_set]

    # For any new driver in the top N without a car, create one.
    for i, (name, score, penalty) in enumerate(top_n):
        if not any(c["name"] == name for c in cars):
            # Default spawn y-position if no replacement info is available.
            spawn_y = 100 + i * 160
            # If previous top N exists and this rank was occupied by a different driver,
            # try to find that car's current y-position.
            if previous_top_n_names and i < len(previous_top_n_names):
                old_name_at_rank = previous_top_n_names[i]
                if old_name_at_rank != name:
                    for c in cars:
                        if c["name"] == old_name_at_rank:
                            spawn_y = c["y"]
                            break
            new_car_img = pygame.image.load(resource_path("assets/car.gif")).convert_alpha()
            new_car_img = pygame.transform.scale(new_car_img, (120, 60))
            new_car = {
                "img": new_car_img,
                "name": name,
                "x": (6 - i) * 200,
                "y": spawn_y,
                "target_x": (6 - i) * 200,
                "target_y": 100 + i * 160,
                "speed": random.randint(1, 3)
            }
            cars.append(new_car)

    # For each car, update its target positions based on its new rank.
    for car in cars:
        try:
            rank = top_n_names.index(car["name"])
            car["target_x"] = (6 - rank) * 200
            car["target_y"] = 100 + rank * 160
        except ValueError:
            pass

    previous_top_n_names = top_n_names.copy()

def animate_cars(speed=5):
    """
    Move each car's x position gradually toward its target_x.
    (Y-axis animation is disabled here but can be enabled if needed.)
    """
    for car in cars:
        if "target_x" in car:
            if car["x"] < car["target_x"]:
                car["x"] += speed
                if car["x"] > car["target_x"]:
                    car["x"] = car["target_x"]
            elif car["x"] > car["target_x"]:
                car["x"] -= speed
                if car["x"] < car["target_x"]:
                    car["x"] = car["target_x"]

def format_time(seconds):
    hours = int(seconds / 3600)
    minutes = (seconds % 3600) // 60
    secs = seconds % 60
    return f"{hours:02d}:{minutes:02d}:{secs:02d}"

def draw_counter(surface):
    """
    Draw the contest timer.
      - If current time is before contest_begin, display "Starts in:".
      - If during contest, display "Time left:".
      - Otherwise, display "Contest ended".
    """
    now = int(time.time())
    if now < contest_begin:
        time_left = contest_begin - now
        time_str = "Starts in: " + format_time(time_left)
    elif now < contest_end:
        time_left = contest_end - now
        time_str = "Time left: " + format_time(time_left)
    else:
        time_str = "Contest ended"
    counter_text = font.render(time_str, True, (255, 255, 255))
    text_width = counter_text.get_width()
    text_height = counter_text.get_height()
    margin = 10
    box_rect = pygame.Rect(WIDTH // 2 - text_width // 2 - margin, 10 - margin, text_width + 2 * margin, text_height + 2 * margin)
    timer_box = pygame.Surface((box_rect.width, box_rect.height), pygame.SRCALPHA)
    timer_box.fill((0, 0, 0, 180))
    surface.blit(timer_box, (box_rect.x, box_rect.y))
    pygame.draw.rect(surface, (255, 255, 255), box_rect, 2, border_radius=5)
    surface.blit(counter_text, (WIDTH // 2 - text_width // 2, 10))

def draw_road(surface):
    global road_x
    surface.blit(road_img, (road_x, 0))
    surface.blit(road_img, (road_x + WIDTH, 0))
    road_x -= road_speed
    if road_x <= -WIDTH:
        road_x = 0

def draw_cars(surface):
    for car in cars:
        surface.blit(car["img"], (car["x"], car["y"]))
        name_text = font.render(car["name"], True, (255, 255, 255))
        surface.blit(name_text, (car["x"], car["y"] - 30))

def draw_leaderboard(surface):
    top_participants = participants_sorted[:15]
    leaderboard_width = 300
    row_height = 35
    header_height = row_height
    content_rows = len(top_participants)
    leaderboard_height = header_height + 1 + content_rows * row_height
    x, y = 10, 100

    lb_surface = pygame.Surface((leaderboard_width, leaderboard_height), pygame.SRCALPHA)
    for i in range(leaderboard_height):
        top_color = (40, 40, 40)
        bottom_color = (10, 10, 10)
        ratio = i / leaderboard_height
        r = int(top_color[0] * (1 - ratio) + bottom_color[0] * ratio)
        g = int(top_color[1] * (1 - ratio) + bottom_color[1] * ratio)
        b = int(top_color[2] * (1 - ratio) + bottom_color[2] * ratio)
        pygame.draw.line(lb_surface, (r, g, b, 150), (0, i), (leaderboard_width, i))
    pygame.draw.rect(lb_surface, (255, 255, 255), (0, 0, leaderboard_width, leaderboard_height), 2)

    leaderboard_title_font = pygame.font.Font(resource_path("assets/alpha.ttf"), size=26)
    leaderboard_rank_font = pygame.font.Font(resource_path("assets/titillium.ttf"), size=24)
    header_text = leaderboard_title_font.render("   Code Sprint", True, (255,255,255))
    lb_surface.blit(header_text, (10, (header_height - header_text.get_height()) // 2))
    pygame.draw.line(lb_surface, (255, 255, 255), (5, header_height), (leaderboard_width - 5, header_height), 2)

    rank_box_width = 40
    arrow_box_width = 25
    points_box_width = 60
    padding = 5

    for idx, (name, score, penalty) in enumerate(top_participants):
        row_y = header_height + 2 + idx * row_height
        color = (255, 255, 255)
        rank_rect = pygame.Rect(padding, row_y + padding, rank_box_width, row_height - 2 * padding)
        rank_text = leaderboard_rank_font.render(str(idx + 1), True, color)
        rank_text_x = rank_rect.x + (rank_rect.width - rank_text.get_width()) // 2
        rank_text_y = rank_rect.y + (rank_rect.height - rank_text.get_height()) // 2
        lb_surface.blit(rank_text, (rank_text_x, rank_text_y))

        arrow_rect = pygame.Rect(rank_rect.right + padding, row_y + padding, arrow_box_width, row_height - 2 * padding)
        arrow_symbol = arrow_states.get(name, '▬')
        if arrow_symbol in ['▲', '↑']:
            arrow_color = (0, 255, 0)
        elif arrow_symbol in ['▼', '↓']:
            arrow_color = (255, 0, 0)
        else:
            arrow_color = (255, 255, 255)
        arrow_text = leaderboard_font.render(arrow_symbol, True, arrow_color)
        arrow_text_x = arrow_rect.x + (arrow_rect.width - arrow_text.get_width()) // 2
        arrow_text_y = arrow_rect.y + (arrow_rect.height - arrow_text.get_height()) // 2
        lb_surface.blit(arrow_text, (arrow_text_x, arrow_text_y))
        
        name_x = arrow_rect.right + padding
        name_text = leaderboard_rank_font.render(name[:14], True, color)
        lb_surface.blit(name_text, (name_x, row_y + (row_height - name_text.get_height()) // 2))
        
        points_rect = pygame.Rect(leaderboard_width - points_box_width - padding, row_y + padding,
                                  points_box_width, row_height - 2 * padding)
        score_text = leaderboard_rank_font.render(str(score), True, color)
        points_text_x = points_rect.x + (points_rect.width - score_text.get_width()) // 2
        points_text_y = points_rect.y + (points_rect.height - score_text.get_height()) // 2
        lb_surface.blit(score_text, (points_text_x, points_text_y))
    
    surface.blit(lb_surface, (x, y))

# def simulate_position_changes(interval=8):
#     global participants_sorted, old_ranking, arrow_states
#     import random, time
#     while True:
#         if len(participants_sorted) > 1:
#             i1, i2 = random.sample(range(len(participants_sorted)), 2)
#             participants_sorted[i1], participants_sorted[i2] = participants_sorted[i2], participants_sorted[i1]
#         new_ranking = [p[0] for p in participants_sorted]
#         for new_index, (name, score, penalty) in enumerate(participants_sorted):
#             if name in old_ranking:
#                 old_index = old_ranking.index(name)
#                 if new_index < old_index:
#                     arrow_states[name] = '↑'
#                 elif new_index > old_index:
#                     arrow_states[name] = '↓'
#                 else:
#                     arrow_states[name] = ''
#             else:
#                 arrow_states[name] = ''
#         old_ranking = new_ranking.copy()
#         time.sleep(interval)

# simulation_thread = threading.Thread(target=simulate_position_changes, args=(8,))
# simulation_thread.daemon = True
# simulation_thread.start()

timer_thread = threading.Thread(target=update_contest_timer)
timer_thread.daemon = True
timer_thread.start()

result_thread = threading.Thread(target=get_result)
result_thread.daemon = True
result_thread.start()

while len(car_names) == 0:
    time.sleep(1)

pygame.init()
info = pygame.display.Info()
desktop_width, desktop_height = info.current_w, info.current_h
screen = pygame.display.set_mode((desktop_width, desktop_height), pygame.NOFRAME)
pygame.display.set_caption("Car Race Betting Game")
WIDTH, HEIGHT = desktop_width, desktop_height
font = pygame.font.SysFont("DejaVu Sans", 36)
leaderboard_font = pygame.font.SysFont("DejaVu Sans", 24)

road_img = pygame.image.load(resource_path("assets/droad.png")).convert()
road_img = pygame.transform.scale(road_img, (WIDTH, HEIGHT))

cars = []

road_x = 0
road_speed = 20
frame_count = 3000

font = pygame.font.Font(resource_path("assets/calibri.ttf"), size=36)
leaderboard_font = pygame.font.Font(resource_path("assets/calibri.ttf"), size=24)

running = True
clock = pygame.time.Clock()

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False

    update_top_cars()
    animate_cars(speed=5)

    screen.fill((255, 255, 255))
    draw_road(screen)
    draw_road(screen)
    draw_cars(screen)
    draw_counter(screen)
    draw_leaderboard(screen)
    pygame.display.flip()
    clock.tick(60)

pygame.quit()
sys.exit()
