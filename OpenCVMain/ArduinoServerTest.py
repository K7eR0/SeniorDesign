import serial
import socket
import time

arduino = serial.Serial(port='COM12',   baudrate=115200, timeout=0.2)

def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.01)
    data = arduino.readline()
    return data

while True:
    num = input("Input number of steps: ")
    value = write_read(num)
    print("Previous Position: ")
    print(value)
    