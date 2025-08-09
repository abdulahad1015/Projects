import pygame
import sys
import random
import requests
import time

# ----------------------------
# Constants
# ----------------------------
WIDTH, HEIGHT = 800, 600
CONTEST_ID = 665719
FPS = 60
REFRESH_INTERVAL = 10000  # in milliseconds (10 seconds)

# ----------------------------
# Initialize Pygame
# ----------------------------
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Code Sprint Leaderboard")
clock = pygame.time.Clock()
font = pygame.font.Font(None, 32)

# ----------------------------
# Load Assets
# ----------------------------
try:
    road_img = pygame.image.load("assets/droad.png").convert()
    car_img = pygame.image.load("assets/car.gif").convert_alpha()
    road_img = pygame.transform.scale(road_img, (WIDTH, HEIGHT))
    car_img = pygame.transform.scale(car_img, (120, 60))
except Exception as e:
    print("Asset loading failed:", e)
    pygame.quit()
    sys.exit()

# ----------------------------
# Game State
# ----------------------------
road_x = 0
cars = []
current_data = {}  # will hold the dictionary returned from backend
last_update = 0

# ----------------------------
# API Handling (Synchronous)
# ----------------------------
def update_leaderboard():
    """Fetch leaderboard data from the backend and update current_data."""
    global current_data, last_update
    try:
        # The backend endpoint expects a contest number and returns a dictionary.
        response = requests.get(f"http://localhost:5000/leaderboard/{CONTEST_ID}", timeout=10)
        if response.status_code == 200:
            current_data = response.json()
            last_update = pygame.time.get_ticks()
            print("Leaderboard updated")
        else:
            print("API response status:", response.status_code)
    except Exception as e:
        print("API Error:", e)

# ----------------------------
# Car Management
# ----------------------------
def initialize_cars():
    """
    Create racer objects based on the leaderboard data.
    If current_data is empty or contains an error, create dummy racers.
    """
    global cars
    cars.clear()

    # If the fetched data is empty or an error message exists, create dummy racers.
    if not current_data or "error" in current_data or len(current_data) == 0:
        for i in range(3):
            cars.append({
                "x": WIDTH + i * 200,
                "y": 400 + i * 70,
                "speed": random.randint(2, 5),
                "name": f"Racer {i+1}"
            })
        return

    # Otherwise, sort the participants by score (desc) then penalty (asc)
    sorted_racers = sorted(current_data.values(), key=lambda x: (-x.get('score', 0), x.get('penalty', 0)))[:3]
    for i, racer in enumerate(sorted_racers):
        cars.append({
            "x": WIDTH + i * 200,
            "y": 400 + i * 70,
            "speed": random.randint(2, 5),
            "name": racer.get("name", f"Racer {i+1}")[:15]  # Limit name length
        })

# ----------------------------
# Drawing Functions
# ----------------------------
def draw_road():
    global road_x
    screen.blit(road_img, (road_x, 0))
    screen.blit(road_img, (road_x + WIDTH, 0))
    # Move the road and loop it.
    road_x = (road_x - 2) % -WIDTH

def draw_cars():
    for car in cars:
        car["x"] -= car["speed"]
        if car["x"] < -200:  # If the car has moved off screen, reset its position
            car["x"] = WIDTH + random.randint(0, 300)
        screen.blit(car_img, (car["x"], car["y"]))
        text = font.render(car["name"], True, (255, 255, 255))
        screen.blit(text, (car["x"] + 10, car["y"] - 30))

def draw_leaderboard():
    """Draw the live leaderboard overlay based on current_data."""
    if not current_data or "error" in current_data:
        return

    overlay = pygame.Surface((300, 400), pygame.SRCALPHA)
    overlay.fill((255, 255, 255, 200))
    title = font.render("Live Leaderboard", True, (0, 0, 0))
    overlay.blit(title, (10, 10))

    # Sort participants by score (desc) and penalty (asc)
    sorted_participants = sorted(current_data.values(),
                                 key=lambda x: (-x.get('score', 0), x.get('penalty', 0)))[:10]

    for i, p in enumerate(sorted_participants):
        display_text = f"{i+1}. {p.get('name','Unknown')[:15]} - {p.get('score',0)} pts"
        color = (200, 0, 0) if i == 0 else (0, 0, 0)
        text = font.render(display_text, True, color)
        overlay.blit(text, (10, 40 + i * 30))
    screen.blit(overlay, (10, 10))

# ----------------------------
# Main Game Loop
# ----------------------------
def main():
    global last_update

    # Fetch the initial leaderboard and initialize cars.
    update_leaderboard()
    initialize_cars()

    running = True
    while running:
        # Event handling.
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Update leaderboard every REFRESH_INTERVAL milliseconds.
        now = pygame.time.get_ticks()
        if now - last_update > REFRESH_INTERVAL:
            update_leaderboard()
            initialize_cars()

        # Drawing
        screen.fill((50, 50, 50))
        draw_road()
        draw_cars()
        draw_leaderboard()

        pygame.display.flip()
        clock.tick(FPS)

    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    main()
