from turtle import Turtle,Screen
import random
import asyncio


async def main():
    while true:
        screen = Screen()
        screen.setup(width=1.0, height=0.9)
        screen.bgpic("assets/img5.gif")
        turtles = []
        draw = Turtle()
        draw.hideturtle()

        root = screen._root
        root.title("Turtle Windowed Mode")
        root.state("zoomed")  #

        # draw.penup()
        # draw.goto(300,-150)
        # draw.pendown()

        racecar_shape = [
            (0, 40), (5, 38), (10, 35), (15, 32), (18, 28),  # Front wing and nose
            (20, 25), (21, 20), (22, 15), (21, 10), (18, 5),  # Front transition
            (15, 0), (12, -5), (10, -10), (8, -15),  # Cockpit start
            (6, -20), (8, -25), (12, -30), (15, -32),  # Body and midsection
            (20, -35), (25, -37), (30, -40), (28, -38),  # Rear wheels right side
            (25, -35), (22, -30), (18, -25), (15, -20),  # Rear wing transition
            (12, -15), (10, -10), (8, -5), (5, 0),  # Cockpit right side

            # Mirror left side for symmetry
            (-5, 0), (-8, -5), (-10, -10), (-12, -15),
            (-15, -20), (-18, -25), (-22, -30), (-25, -35),
            (-28, -38), (-30, -40), (-25, -37), (-20, -35),
            (-15, -32), (-12, -30), (-8, -25), (-6, -20),
            (-8, -15), (-10, -10), (-12, -5), (-15, 0),
            (-18, 5), (-21, 10), (-22, 15), (-21, 20),
            (-20, 25), (-18, 28), (-15, 32), (-10, 35),
            (-5, 38), (0, 40),  # Close the front and mirror

            # Wheels (front and rear)
            (30, 5), (32, 10), (32, 20), (30, 25),  # Front right wheel
            (-30, 5), (-32, 10), (-32, 20), (-30, 25),  # Front left wheel
            (30, -30), (32, -35), (32, -45), (30, -50),  # Rear right wheel
            (-30, -30), (-32, -35), (-32, -45), (-30, -50),  # Rear left wheel

            # Rear wing bottom span
            (-25, -45), (-20, -48), (-15, -50), (15, -50), (20, -48), (25, -45)
        ]

        # Register the custom shape

        # Use the shape and display it

        # for i in range(-150,150,10):
        #     if((i/10)%2==0):
        #         draw.pendown()
        #     else:
        #         draw.penup()
        #     draw.goto(300,i)

        colours = ['red', 'yellow', 'green', 'blue', 'purple']
        y_cord = -300
        for i in range(4):
            turtler = Turtle()
            screen.register_shape("img3.gif")
            turtler.shape("img3.gif")
            turtler.turtlesize(stretch_wid=0.5, stretch_len=0.5, outline=0.5)
            turtles.append(turtler)
            # turtles[i].color(colours[i])
            turtles[i].penup()
        for i in range(3, -1, -1):
            turtles[i].goto(-600, y_cord)
            y_cord += 200

        user_bet = screen.textinput("Make your bet", "Which turtle will win the race ? Enter a Color: ")

        race_end = 0
        while (race_end == 0):
            race_end = 1
            for i in turtles:
                pos = i.xcor()
                rand = random.randint(0, 10)
                if (pos < 450):
                    race_end = 0
                    if (rand + pos > 450):
                        i.goto(450, i.ycor())
                    else:
                        i.forward(rand)
                else:
                    if (i.color()[0] == user_bet):
                        print("You won the bet")
                    else:
                        print(i.color()[0], " Won !!!")
                        print("You Lost the bet")
                    race_end = 1
                    break
        screen.exitonclick()

asyncio.run(main())