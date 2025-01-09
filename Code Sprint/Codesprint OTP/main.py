022211
import time
from tkinter import *
from tkinter import messagebox
import random
import pyperclip
import pyotp
# ---------------------------- PASSWORD GENERATOR ------------------------------- #
letters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
numbers = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
symbols = ['!', '#', '$', '%', '&', '(', ')', '*', '+']

nr_letters = random.randint(8, 10)
nr_symbols = random.randint(2, 4)
nr_numbers = random.randint(2, 4)

def generate_6_digit_totp(key):
    print(time.time())
    totp = pyotp.TOTP(key)
    return totp.now()
def generate_password():
    # password_list = []
    # for char in range(nr_letters):
    #     password_list.append(random.choice(letters))
    # for char in range(nr_symbols):
    #     password_list += random.choice(symbols)
    # for char in range(nr_numbers):
    #     password_list += random.choice(numbers)
    # random.shuffle(password_list)
    password = generate_6_digit_totp(Entry1.get())

    # for char in password_list:
    #     password += char

    Entry3.delete(0,END)
    Entry3.insert(0, password)
    pyperclip.copy(password)
# ---------------------------- SAVE PASSWORD ------------------------------- #
# def save():
#     website=Entry1.get()
#     email=Entry2.get()
#     password=Entry3.get()
#     if (len(website) == 0 or len(email) == 0 or len(password) == 0):
#         messagebox.showinfo(title="Error ",message="Please Enter complete details.")
#     else:
#         is_ok=messagebox.askokcancel(title=website,message=f"These are the details entered: \nEmail: {Entry2.get()} \nPassword: {Entry3.get()}\n\nContinue ?")
#
#         if is_ok:
#             with open("passwords.txt", "a") as file:
#                 file.write(f"{website} | {Entry2.get()} | {Entry3.get()}\n")
#             Entry1.delete(0,END)
#             Entry2.delete(0,END)
#             Entry2.insert(0, "abdulahad1015@gmail.com")
#             Entry3.delete(0,END)
#             Entry1.focus()
#             #messagebox.showinfo(title="Title",message="Data Saved")


# ---------------------------- UI SETUP ------------------------------- #
window=Tk()
window.title("Password manager")
window.config(padx=50,pady=60)

lock_img=PhotoImage(file="logo.png")

canvas = Canvas(width=150,height=200)
canvas.create_image(89,100,image=lock_img)
canvas.grid(row=0,column=1)
#-------------------------Entries-----------------------------
Entry1=Entry(width=44)
Entry1.grid(row=1,column=1,columnspan=2,sticky="w")
Entry1.focus()


# Entry2=Entry(width=44)
# Entry2.grid(row=2,column=1,columnspan=2,sticky="w")
# Entry2.insert(0,"abdulahad1015@gmail.com")

Entry3=Entry(width=25)
Entry3.grid(row=3,column=1,sticky="w")
#--------------------------Labels------------------------------
Label1=Label(text="Enter Password:")
Label1.grid(row=1,column=0,sticky="w")

# Label2=Label(text="Email/Username:")
# Label2.grid(row=2,column=0,sticky="w")

Label3=Label(text="Code:")
Label3.grid(row=3,column=0,sticky="w")
#--------------------------Buttons------------------------------
Button1=Button(text="Generate Code",width=15,command=generate_password)
Button1.grid(row=3,column=2,sticky="w")

# Button2=Button(text="Add",width=37,command=save)
# Button2.grid(row=4,column=1,columnspan=2,sticky="w")

window.mainloop()