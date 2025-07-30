import serial
import time
import threading
import cv2
import mediapipe as mp
import numpy as np
import queue

uno = serial.Serial(port='COM12',   baudrate=500000 , timeout=0.02) # Start Serial connection with Uno
nano = serial.Serial(port='COM3',   baudrate=500000 , timeout=0.02) # Start Serial connection with Nano

while True:
    try:
        # Read line, decode, and strip newlines
        raw_data = nano.readline()
        if raw_data: # Only process if data was received
            decoded_data = raw_data.decode('utf-8').strip()
            if decoded_data: # Only put valid, non-empty data into the queue
                uno.write(decoded_data.encode())
        # Small sleep to prevent 100% CPU usage on the thread if data is very sparse
        # time.sleep(0.001)
    except serial.SerialException as e:
        print(f"Serial read error on Nano: {e}")
        break # Exit thread on critical serial error
    except UnicodeDecodeError as e:
        print(f"Unicode decode error on Nano: {e}. Raw data: {raw_data}")
        # Continue trying to read
    except Exception as e:
        print(f"Unexpected error in Nano read thread: {e}")
        break # Exit thread on unexpected error