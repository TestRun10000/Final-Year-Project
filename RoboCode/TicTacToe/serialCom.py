import serial


class com:
    def __init__(self, portName, baudrate=9600):
        self.port = serial.Serial()
        self.port.port = portName
        self.port.baudrate = baudrate
        self.port.timeout = 1
        self.port.open()

    def readText(self):
        return self.port.readline().decode()

    def sendText(self, message):
        message = message + "\n"
        self.port.write(message.encode("utf-8"))
