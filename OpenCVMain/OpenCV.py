import serial
import time
import threading
import cv2
import mediapipe as mp
import numpy as np
import queue

uno = serial.Serial(port='COM12',   baudrate=500000 , timeout=0.02) # Start Serial connection with Uno

def OpenCV(wristX,wristY,elbowX,elbowY,shoulderX,shoulderY):
    mp_drawing = mp.solutions.drawing_utils # Start the drawing utility to draw on top of a frame
    mp_pose = mp.solutions.pose # Start the pose library to detect poses
    cap = cv2.VideoCapture(0) # Start the video capture, change number to change camera
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
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
            wristXTemp =landmarks[mp_pose.PoseLandmark.RIGHT_WRIST].x
            wristYTemp =landmarks[mp_pose.PoseLandmark.RIGHT_WRIST].y
            elbowXTemp =landmarks[mp_pose.PoseLandmark.RIGHT_ELBOW].x
            elbowYTemp =landmarks[mp_pose.PoseLandmark.RIGHT_ELBOW].y
            shoulderXTemp =landmarks[mp_pose.PoseLandmark.RIGHT_SHOULDER].x
            shoulderYTemp =landmarks[mp_pose.PoseLandmark.RIGHT_SHOULDER].y
            wristX.put(wristXTemp)
            wristY.put(wristYTemp)
            elbowX.put(elbowXTemp)
            elbowY.put(elbowYTemp)
            shoulderX.put(shoulderXTemp)
            shoulderY.put(shoulderYTemp)
            #Draw joints
            mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
            cv2.imshow('Mediapipe Feed', image) # Display image
            if cv2.waitKey(10) & 0xFF == ord('q'): # If q key is pressed
                break
        cap.release() # Release control of camera
        cv2.destroyAllWindows() # Destroy all windows
    
wristX = queue.Queue()
wristY = queue.Queue()
elbowX = queue.Queue()
elbowY = queue.Queue()
shoulderX = queue.Queue()
shoulderY = queue.Queue()
t1 = threading.Thread(target=OpenCV, args=(wristX,wristY,elbowX,elbowY,shoulderX,shoulderY,)) # Create a thread for OpenCV
t1.start()       

while True:
    #Create message to send to Uno
    wristX.queue.clear()
    wristY.queue.clear()
    elbowX.queue.clear()
    elbowY.queue.clear()
    shoulderX.queue.clear()
    shoulderY.queue.clear()
    total = f"{wristX.get():6.2f} {wristY.get():6.2f} {elbowX.get():6.2f} {elbowY.get():6.2f} {shoulderX.get():6.2f} {shoulderY.get():6.2f} "
    
    #Print message to console
    print(total)
    
    #Send message to Uno
    uno.write(total.encode())
    time.sleep(0.1)