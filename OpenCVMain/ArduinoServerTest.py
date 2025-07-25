import serial
import time
import threading
import cv2
import mediapipe as mp
import numpy as np
import queue

uno = serial.Serial(port='COM12',   baudrate=2000000, timeout=0.2) # Start Serial connection with Uno
nano = serial.Serial(port='COM3',   baudrate=2000000, timeout=0.2) # Start Serial connection with Nano

def ReadNano(package): # Function to receive and process coordinates from nano
    pack = nano.readline()[:-2].decode() # Decode the message from the nano and remove the new line command
    # print(package) # Print the message from the nano
    uno.write(pack.encode())
    package.put(pack) # Put package into queue 

    
package = queue.Queue() # Create queue to store nano value
while True:     
    mp_drawing = mp.solutions.drawing_utils # Start the drawing utility to draw on top of a frame
    mp_pose = mp.solutions.pose # Start the pose library to detect poses
    cap = cv2.VideoCapture(0) # Start the video capture, change number to change camera
    with mp_pose.Pose(min_detection_confidence = 0.7, min_tracking_confidence = 0.7) as pose: # Set pose detection confidence and puts a variable pose
        while cap.isOpened(): # While the camera is open
            ret, frame = cap.read() # Store the current frame
            
            #Change color space
            image = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB) # Change the color space from BGR to RGB for mediapipe
            image.flags.writeable = False # Saves memory
            
            #Detection
            results = pose.process(image) # Process the image in mediapipe
            
            #Change color space back
            image.flags.writeable = True
            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR) # Changes the color space from RGB to BGR for OpenCV to render
            
            #Extract landmarks
            try:
                landmarks = (results.pose_landmarks.landmark)
            except:
                pass
            
            #Get message from threaded nano
            t1 = threading.Thread(target=ReadNano, args=(package,)) # Create a thread for nano
            t1.start()  # Start the thread
            print(package.get())
            print("\n")
            print(landmarks[mp_pose.PoseLandmark.RIGHT_WRIST].x)
            print("\n")
            
            
            
            #Draw joints
            mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
            
            cv2.imshow('Mediapipe Feed', image) # Display image
            if cv2.waitKey(10) & 0xFF == ord('q'): # If q key is pressed
                break
        cap.release() # Release control of camera
        cv2.destroyAllWindows() # Destroy all windows

