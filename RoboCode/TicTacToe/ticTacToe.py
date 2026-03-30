import numpy as np
import math
import copy


class point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def getX(self):
        return self.x

    def getY(self):
        return self.y

    def __str__(self):
        return "(" + str(self.x) + "," + str(self.y) + ")"

    def __eq__(self, value) -> bool:
        return self.x == value.getX() and self.y == value.getY()


class ticTacToeBoard:
    def __init__(self):
        self.board = np.zeros((3, 3), np.int8)
        self.freeSpace = [point(x, y) for x in range(3) for y in range(3)]

    def place(self, pos, player):
        if pos in self.freeSpace:
            self.freeSpace.remove(pos)
            self.board[pos.getX()][pos.getY()] = player
            return True
        else:
            return False

    def hasWon(self, player):
        playerSet = [player for _ in range(3)]
        if np.any(np.all(playerSet == self.board[:, :], axis=1)):
            return True
        elif np.any(np.all(playerSet == self.board[:, :], axis=0)):
            return True
        elif self.board[1, 1] == player:
            if self.board[0, 0] == player and self.board[2, 2] == player:
                return True
            elif self.board[0, 2] == player and self.board[2, 0] == player:
                return True
        return False

    def isGameOver(self):
        return self.hasWon(1) or self.hasWon(2) or self.movesLeft() == 0

    def movesLeft(self):
        return len(self.freeSpace)

    def getAvailableMoves(self):
        return self.freeSpace

    def copy(self):
        nCopy = ticTacToeBoard()
        nCopy.board = copy.deepcopy(self.board)
        nCopy.freeSpace = copy.deepcopy(self.freeSpace)
        return nCopy


class ticTacToeGame:
    def __init__(self):
        self.whosGo = 1
        self.board = ticTacToeBoard()
        self.ai = 2
        self.human = 1

    def makeAiMove(self):
        print(self.board.board)
        pos = self.minmax(1, self.board)[0]
        self.board.place(pos, self.ai)
        print(self.board.board)
        return pos

    # uses negaMax
    def place(self, row, col, player):
        return self.board.place(point(row, col), player)

    def minmax(self, color, board):
        if board.isGameOver():
            score = 0
            if board.hasWon(self.ai):
                score = color * 10
            elif board.hasWon(self.human):
                score = color * -10
            return (point(0, 0), score)
        value = -math.inf
        pos = point(0, 0)
        player = self.ai if color > 0 else self.human
        for i in board.getAvailableMoves():
            child = board.copy()
            child.place(i, player)
            res = -self.minmax(-color, child)[1]
            if value < res:
                pos = i
                value = res
        return (pos, value)

    def isGameOver(self):
        return self.board.isGameOver()

    def humanMove(self, pos):
        self.board.place(pos, self.human)
