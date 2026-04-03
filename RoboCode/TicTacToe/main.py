import customtkinter as ct
import ticTacToe
import time


def button_callback(row, col):

    print(f"button({row},{col})")


class game(ct.CTkFrame):
    def __init__(self):
        self.cross = "❌"
        self.naught = "⭕"

        self.grid_rowconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.text = ct.CTkLabel(self, height=100, text="Players Turn ❌")
        self.text.grid(row=0, column=0, columnspan=3, padx=20, pady=20, sticky="nsew")

        frame = ct.CTkFrame(self, width=315, height=315)
        frame.grid(row=1, column=0, padx=20, pady=20, sticky="nsew")
        for i in range(3):
            frame.grid_rowconfigure(i, weight=1, uniform="group1")
            frame.grid_columnconfigure(i, weight=1, uniform="group2")

        buttonIndex = 1
        self.buttons = []
        for row in range(3):
            for col in range(3):
                btn = ct.CTkButton(
                    frame,
                    text=" ",
                    command=lambda r=row, c=col: self.buttonPress(r, c),
                    width=90,
                    height=90,
                    bg_color="white",
                    fg_color="white",
                    hover=False,
                    text_color="black",
                )
                btn._text = "X"
                self.buttons.append(btn)
                btn.grid(row=row, column=col, padx=5, pady=5, sticky="nsew")
                buttonIndex += 1
        self.game = ticTacToe.ticTacToeGame()
        self.HumansGo = True
        self.bind("<Configure>", self.on_resize)
        self.after(100, self.gameLoop)

    def buttonPress(self, row, col):
        if self.HumansGo:
            if self.game.place(row, col, 1):
                self.buttons[row * 3 + col].configure(text="❌", text_color="red")
                self.HumansGo = False
        pass

    def on_resize(self, event):
        # Get current window width
        width = self.winfo_width()
        self.text.configure(font=("Arial", width / 25))
        # Adjust UI based on width
        for i in self.buttons:
            i.configure(font=("Arial", width / 15))

    def gameLoop(self):
        if not self.game.isGameOver():
            if not self.HumansGo:
                self.text.configure(text="Players Turn ⭕")
                time.sleep(0.5)
                self.update()
                pos = self.game.makeAiMove()
                self.buttons[pos.x * 3 + pos.y].configure(
                    text=self.naught, text_color="blue"
                )
                self.HumansGo = True
            else:
                self.text.configure(text="Players Turn ❌")
            self.after(100, self.gameLoop)


class homePage:
    def __init__(self):
        pass
