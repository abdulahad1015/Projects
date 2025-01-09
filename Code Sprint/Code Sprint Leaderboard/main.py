# pyinstaller --onefile --windowed --hidden-import=pygame --add-data "img3.gif:." --add-data "img.png:." --add-data "calibri.ttf:." main.py

import pygame,random,sys,requests,time,threading,os
from result import get_participants
CONTEST_NO = 665719
critical = True
car_names=[]
def MySortFunc(a):
    return a[1]

def resource_path(relative_path):
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

def get_result():
    global car_names,critical
    count=0
    while True:
        participants = get_participants(CONTEST_NO)
        # participants.
        list=[]
        a=participants.values()
        for i in a:
            list.append((i['name'],i['score'],i['penalty']))
        else:
            list = [i[0] for i in sorted(list,key=lambda i:(i[1],-i[2]),reverse=True)]
        print(list)
        critical=False
        car_names=list
        critical=True
        count+=1
        time.sleep(5)


# Thread initialization
thread = threading.Thread(target=get_result)
thread.daemon = True  # Ensures the thread exits when the main program ends.
thread.start()
time.sleep(5)

# Initialize Pygame
pygame.init()
# Set up the screen in fullscreen mode
# screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
screen = pygame.display.set_mode((800, 600))
WIDTH, HEIGHT = screen.get_size()
pygame.display.set_caption("Car Race Betting Game")

# Load and scale the road image
road_img = pygame.image.load(resource_path("img.png")).convert()
road_img = pygame.transform.scale(road_img, (WIDTH, HEIGHT))

# Car data (including names)
cars=len(car_names)
# Load and scale the car images
car_imgs = [pygame.image.load(resource_path("img3.gif")).convert_alpha() for i in range(0, cars)]
car_imgs = [pygame.transform.scale(car, (120, 60)) for car in car_imgs]  # Resize for horizontal racing


cars = [{"img": car, "name": car_names[i], "x": (6-i)*200,
         "y": 100 + i * 160, "speed": random.randint(1, 3)}
        for i, car in enumerate(car_imgs)]

# Road scrolling variables
road_x = 0
road_speed = 20
frame_count = 3000

# Font for counter and names
font = pygame.font.Font(resource_path("calibri.ttf"),size=36)  # Use default Pygame font, size 36





def draw_road():
    """Draw the horizontally scrolling road from right to left."""
    global road_x
    screen.blit(road_img, (road_x, 0))
    screen.blit(road_img, (road_x + WIDTH, 0))  # Second road follows the first

    road_x -= road_speed  # Move the road to the left

    # Reset when the road is off-screen
    if road_x <= -WIDTH:
        road_x = 0

def draw_cars():
    """Draw the cars with their names."""
    for car in cars:
        # Draw the car
        screen.blit(car["img"], (car["x"], car["y"]))
        # Render the name above each car
        name_text = font.render(car["name"], True, (255, 255, 255))
        screen.blit(name_text, (car["x"], car["y"] - 30))  # Name above the car

def update_car_positions():
    """Move cars from left to right."""
    global critical
    if(critical):
        for car in cars:
            car["x"] = (6-car_names.index(car["name"]))*200
            # if car["x"] > WIDTH:  # Reset when it moves off-screen
            #     car["x"] = random.randint(-200, -50)
            #     car["speed"] = random.randint(5, 10)
    else:
        return

def draw_counter():
    """Draw the frame counter at the top of the screen."""
    global frame_count
    frame_count -= 1/40  # Increment counter every frame

    # Render the counter text
    if frame_count%60<10:
        counter_text = font.render(f'''Time: {(int(frame_count / 60))}:0{int(frame_count % 60)}''', True,
                                   (255, 255, 255))
    else:
        counter_text = font.render(f'''Time: {(int(frame_count/60))}:{int(frame_count%60)}''', True, (255, 255, 255))
    screen.blit(counter_text, (WIDTH // 2 - counter_text.get_width() // 2, 10))  # Centered at the top


clock = pygame.time.Clock()
running = True

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT :
            pygame.quit()
            sys.exit()

    # Update the game state
    screen.fill((255, 255, 255))  # Optional background color


    draw_road()  # Draw scrolling road
    draw_road()  # Draw scrolling road

    draw_cars()  # Draw cars with names

    update_car_positions()  # Update car positions

    draw_counter()  # Draw the counter


    pygame.display.flip()
    clock.tick(60)  # Limit to 30 FPS

# Run the game
