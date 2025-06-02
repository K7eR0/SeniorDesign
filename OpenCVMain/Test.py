import cv2
import mediapipe as mp

#m1 = board.get_pin()

# Initialize MediaPipe Holistic model
mp_holistic = mp.solutions.holistic
holistic = mp_holistic.Holistic(min_detection_confidence=0.7, min_tracking_confidence=0.7)

# Initialize OpenCV video capture
cap = cv2.VideoCapture(0)
while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Flip the frame horizontally for better mirror effect
    frame = cv2.flip(frame, 1)

    # Convert the frame to RGB (MediaPipe uses RGB)
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Process the frame with MediaPipe Holistic
    results = holistic.process(rgb_frame)

    # Draw the hand landmarks

    if results.right_hand_landmarks:
        for i in range(len(results.right_hand_landmarks.landmark)):
            x = int(results.right_hand_landmarks.landmark[i].x * frame.shape[1])
            y = int(results.right_hand_landmarks.landmark[i].y * frame.shape[0])
            cv2.circle(frame, (x, y), 5, (0, 255, 0), -1)

        # Connect the hand landmarks (for better visualization)
        mp.solutions.drawing_utils.draw_landmarks(frame, results.right_hand_landmarks, mp_holistic.HAND_CONNECTIONS)

    # Draw the arm and shoulder landmarks (using Pose landmarks)
    if results.pose_landmarks:
        # Define the index of relevant points (shoulder, elbow, wrist)
        shoulder_right = results.pose_landmarks.landmark[mp_holistic.PoseLandmark.RIGHT_SHOULDER]
        elbow_right = results.pose_landmarks.landmark[mp_holistic.PoseLandmark.RIGHT_ELBOW]
        wrist_right = results.pose_landmarks.landmark[mp_holistic.PoseLandmark.RIGHT_WRIST]

        # Convert normalized coordinates to pixel coordinates

        shoulder_right_coords = (int(shoulder_right.x * frame.shape[1]), int(shoulder_right.y * frame.shape[0]))
        elbow_right_coords = (int(elbow_right.x * frame.shape[1]), int(elbow_right.y * frame.shape[0]))
        wrist_right_coords = (int(wrist_right.x * frame.shape[1]), int(wrist_right.y * frame.shape[0]))

        # Draw circles for shoulder, elbow, and wrist

        cv2.circle(frame, shoulder_right_coords, 5, (0, 0, 255), -1)
        cv2.circle(frame, elbow_right_coords, 5, (0, 0, 255), -1)
        cv2.circle(frame, wrist_right_coords, 5, (0, 0, 255), -1)

        # Draw lines to represent arm connections (shoulder -> elbow -> wrist)

        cv2.line(frame, shoulder_right_coords, elbow_right_coords, (0, 0, 255), 2)
        cv2.line(frame, elbow_right_coords, wrist_right_coords, (0, 0, 255), 2)

    # Display the resulting frame
    cv2.imshow('Hand and Arm Tracking', frame)
    
    # Exit on pressing the 'q' key
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the video capture object and close OpenCV windows
cap.release()
cv2.destroyAllWindows()
