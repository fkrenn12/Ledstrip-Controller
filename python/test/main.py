from random import randint

from serial import Serial
import json
import time
import random

COMMAND_LINES = [
    {"strips": [1, 2], "update-mode": "instant", "pixels": 256, "brightness": 2.0, "repeat": 0,
     "pattern": [randint(1, 21), randint(22, 44), randint(44, 63)], "animation": 1, "animation-mode": "rotate-right"},
]


def send(command):
    s.write(json.dumps(command).encode() + '\n'.encode())
    print(f"Sent command: {command}")
    time.sleep(0.001)  # avoids rx buffer overrun in controller


def main():

    while True:
        send({"animation": 1})
        # setup and clear strip#1
        send({"strip": 1, "update-mode": "instant", "pixels": 768, "pattern": [0], "repeat": 0, "brightness": 4})
        # setup and clear strip#2
        send({"strip": 2, "update-mode": "instant", "pixels": 8, "pattern": [0], "repeat": 0,
              "brightness": 1, "interval": 100, "animation-mode": "rotate-right"})
        send({"strip": 2, "repeat": 1, "pattern": [3]})
        # setup and clear strip#3
        send({"strip": 3, "update-mode": "instant", "pixels": 8, "pattern": [0], "repeat": 0, "brightness": 4})

        for i in range(1000000000):
            try:
                send({"strip": 1, "repeat": 1, "pattern-add": [randint(1, 21)], "first": randint(0, 768)})
                send({"strip": 3, "repeat": 2, "pattern": [randint(1, 63)], "first": randint(1, 7)})
                if i >100:
                    send({"strip": 1, "repeat": 4, "pattern-add": [0], "first": randint(0, 768)})
            except Exception as e:
                print(f"Error sending command")
            finally:
                time.sleep(0.01)


if __name__ == "__main__":
    s = Serial("COM5", 921600, timeout=1)
    main()
