import serial
import time
import threading
import cv2
import mediapipe as mp
import numpy as np
import queue

uno = serial.Serial(port='COM12',   baudrate=500000 , timeout=0.02) # Start Serial connection with Uno
nano = serial.Serial(port='COM3',   baudrate=500000 , timeout=0.02) # Start Serial connection with Nano

def ReadNano(package): # Function to receive and process coordinates from nano
    while True:
        try:
            # Read line, decode, and strip newlines
            raw_data = nano.readline()
            if raw_data: # Only process if data was received
                decoded_data = raw_data.decode('utf-8').rstrip()
                if decoded_data: # Only put valid, non-empty data into the queue
                    package.put(decoded_data)
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


# Create queues to store values
package = queue.Queue()
wristX = queue.Queue()
wristY = queue.Queue()
elbowX = queue.Queue()
elbowY = queue.Queue()
shoulderX = queue.Queue()
shoulderY = queue.Queue()

t1 = threading.Thread(target=ReadNano, args=(package,)) # Create a thread for nano
t1.start()
t2 = threading.Thread(target=OpenCV, args=(wristX,wristY,elbowX,elbowY,shoulderX,shoulderY,)) # Create a thread for OpenCV
t2.start()            

while True:
    package.queue.clear()
    wristX.queue.clear()
    wristY.queue.clear()
    elbowX.queue.clear()
    elbowY.queue.clear()
    shoulderX.queue.clear()
    shoulderY.queue.clear()
    #Create message to send to Uno
    total = f"{wristX.get():6.2f} {wristY.get():6.2f} {elbowX.get():6.2f} {elbowY.get():6.2f} {shoulderX.get():6.2f} {shoulderY.get():6.2f} " + package.get()
    
    #Print message to console
    print(total)
    
    #Send message to Uno
    uno.write(total.encode())
    print(uno.readline().decode())
    time.sleep(0.025)