import serial
import socket
import time
import threading

arduino = serial.Serial(port='COM12',   baudrate=115200, timeout=0.2)
nano = serial.Serial(port='COM3',   baudrate=115200, timeout=0.2)

def readnano():
    while True:
        package = nano.readline()[:-2].decode() # Decode the message from the nano and remove the new line command
        arduino.write(package.encode()) # Re-encode the message and send to arduino
        print(package) # Print the message from the nano
t = threading.Thread(target=readnano)
t.start()    