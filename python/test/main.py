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


def main():

    while True:
        send({"strips": [1, 2], "update-mode": "instant", "pixels": 256, "pattern": [0], "repeat": 0,
              "brightness": 0.08, "interval": 100, "animation": 0, "animation-mode": "rotate-right"})
        for i in range(50):
            try:
                command = {"strips": [1, 2], "repeat": 1, "pixels": 256, "brightness": 1.08, "interval": 100,
                           "pattern-add": [randint(1, 21)], "first": randint(0, 256)}

                # command = {"strips": [1, 2], "repeat": 1,
                #           "pattern-add": [randint(1, 21), randint(22, 44), randint(44, 63)], "first": randint(0, 256) }
                send(command)
            except Exception as e:
                print(f"Error sending command {command}: {e}")
            finally:
                time.sleep(0.05)


if __name__ == "__main__":
    s = Serial("COM5", 921600, timeout=1)
    main()
