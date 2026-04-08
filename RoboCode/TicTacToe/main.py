import customtkinter as ct
import ticTacToe
import threading


FELT_GREEN = "#2D5A27"


class game(ct.CTkFrame):
    def __init__(self, master, returnFunc):
        super().__init__(master, fg_color=FELT_GREEN)
        self.cross = "❌"
        self.naught = "⭕"

        self.grid_rowconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=1)

        self.text = ct.CTkLabel(self, height=100, text="Players Turn ❌")
        self.text.grid(row=0, column=0, columnspan=3, padx=20, pady=20, sticky="nsew")
        self.retFunc = returnFunc

        frame = ct.CTkFrame(self, width=315, height=315, corner_radius=15)
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
                    bg_color="transparent",
                    fg_color="white",
                    hover=False,
                    text_color="black",
                    corner_radius=15,
                )
                btn._text = "X"
                self.buttons.append(btn)
                btn.grid(row=row, column=col, padx=5, pady=5, sticky="nsew")
                buttonIndex += 1
        self.game = ticTacToe.ticTacToeGame()
        self.HumansGo = True
        self.bind("<Configure>", self.on_resize)

    def buttonPress(self, row, col):
        if self.HumansGo and not self.game.isGameOver():
            if self.game.place(row, col, 1):
                self.buttons[row * 3 + col].configure(text="❌", text_color="red")
                self.update()
                self.HumansGo = False
                if not self.game.isGameOver():
                    self.aiPlay()
        if self.game.isGameOver():
            self.gameOver()

    def aiPlay(self):
        self.text.configure(text="Players Turn ⭕")
        threading.Thread(target=self.aiTurn, daemon=True).start()

    def aiTurn(self):
        pos = self.game.makeAiMove()
        self.after(0, self.updateAfterAi, pos)

    def updateAfterAi(self, pos):
        self.buttons[pos.x * 3 + pos.y].configure(text=self.naught, text_color="blue")
        self.HumansGo = True
        self.text.configure(text="Players Turn ❌")

    def on_resize(self, event):
        # Get current window width
        width = self.winfo_width()
        self.text.configure(font=("Arial", width / 25))
        # Adjust UI based on width
        for i in self.buttons:
            i.configure(font=("Arial", width / 15))

    def gameOver(self):
        self.frame = ct.CTkFrame(self, fg_color="transparent")
        self.frame.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")
        self.frame.grid_columnconfigure(0, weight=1)
        self.frame.grid_rowconfigure(0, weight=1)
        self.frame.grid_rowconfigure(1, weight=1)
        label = ct.CTkLabel(
            self.frame, text="Game Over", font=("Arial", self.winfo_width() / 25)
        )
        label.grid(row=0, column=0, sticky="nsew")
        button = ct.CTkButton(
            self.frame,
            text="return",
            command=self.exitGame,
            font=("Arial", self.winfo_width() / 25),
        )
        button.grid(row=1, column=0, sticky="ns")

    def exitGame(self):
        self.HumansGo = True
        self.frame.destroy()
        self.game = ticTacToe.ticTacToeGame()
        for i in self.buttons:
            i.configure(text="")
        print(self.game.board.board)
        self.retFunc()


class mainApp(ct.CTk):
    def __init__(self):
        super().__init__(fg_color=FELT_GREEN)
        self.title("Solitaire Bot")
        self.home = ct.CTkFrame(self, width=315, height=315, fg_color="transparent")
        self.geometry("600x600")
        self.minsize(600, 600)
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)

        self.home.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")

        self.home.grid_columnconfigure(0, weight=1)
        self.home.grid_rowconfigure(0, weight=1)
        self.home.grid_rowconfigure(1, weight=1)
        self.home.grid_rowconfigure(2, weight=1)  #

        self.btn1 = ct.CTkButton(self.home, text="Homming", command=self.GoToZero)
        self.btn1.grid(row=0, column=0, padx=20, pady=10, sticky="nsew")

        self.btn2 = ct.CTkButton(self.home, text="Write", command=self.Write)
        self.btn2.grid(row=1, column=0, padx=20, pady=10, sticky="nsew")

        self.btn3 = ct.CTkButton(self.home, text="Play", command=self.play)
        self.btn3.grid(row=2, column=0, padx=20, pady=10, sticky="nsew")
        self.gameFrame = game(self, self.retFunc)
        self.gameFrame.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")
        self.framse = [self.home, self.gameFrame]
        self.home.tkraise()

    def GoToZero(self):
        print("Homming")

    def Write(self):
        print("Writing")

    def play(self):
        print("Game")
        self.framse[1].tkraise()
        self.update()

    def retFunc(self):
        self.framse[0].tkraise()
        self.update()
        print("return")


app = mainApp()
app.mainloop()
