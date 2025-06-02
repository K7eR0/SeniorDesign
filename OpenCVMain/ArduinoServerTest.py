import serial
import socket
import time

arduino = serial.Serial(port='COM12',   baudrate=115200, timeout=.1)

def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = arduino.readline()
    return data

while True:
    num = input("Input a GPIO pin and state \n examples: (01 u) or (02 d)")
    value = write_read(num)
    print(value)
    