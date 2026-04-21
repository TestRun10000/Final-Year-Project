import serial
import time


class com:
    def __init__(self, portName, baudrate=9600):
        self.port = serial.Serial()
        self.port.port = portName
        self.port.baudrate = baudrate
        self.port.timeout = 1
        self.port.open()
        time.sleep(2)

    def readText(self):
        return self.port.readline().decode().strip()

    def sendText(self, message):
        if message[-1] != "\n":
            message = message + "\r\n"
        self.port.write(message.encode("utf-8"))


class falseCom:
    def __init__(self):
        pass

    def readText(self):
        return "done"

    def sendText(self, message):
        if message[-1] != "\n":
            message = message + "\n"
        print(message.encode("utf-8"))
