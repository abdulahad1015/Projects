import pygame
import sys

# Initialize Pygame
pygame.init()

# Screen dimensions
WIDTH, HEIGHT = 800, 600

# Create the screen
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Pygame with Pygbag")

# Colors
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)

# Clock for controlling frame rate
clock = pygame.time.Clock()

# Game Loop
def main():
    running = True
    while running:
        # Event Handling
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Game Logic
        screen.fill(WHITE)  # Clear the screen
        pygame.draw.circle(screen, BLUE, (WIDTH // 2, HEIGHT // 2), 50)  # Draw a circle

        # Update Display
        pygame.display.flip()

        # Limit frame rate to 60 FPS
        clock.tick(60)

    # Clean up
    pygame.quit()
    sys.exit()

# Entry Point
if __name__ == "__main2__":
    main()
