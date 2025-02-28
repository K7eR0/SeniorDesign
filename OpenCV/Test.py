import cv2
import mediapipe as mp

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

    # Draw the left hand landmarks
    if results.left_hand_landmarks:
        for i in range(len(results.left_hand_landmarks.landmark)):
            x = int(results.left_hand_landmarks.landmark[i].x * frame.shape[1])
            y = int(results.left_hand_landmarks.landmark[i].y * frame.shape[0])
            cv2.circle(frame, (x, y), 5, (0, 255, 0), -1)

        # Connect the left hand landmarks (for better visualization)
        mp.solutions.drawing_utils.draw_landmarks(frame, results.left_hand_landmarks, mp_holistic.HAND_CONNECTIONS)

    # Draw the left arm and wrist landmarks (using Pose landmarks) up to the elbow
    if results.pose_landmarks:
        # Define the index of relevant points (left wrist, left elbow)
        wrist_left = results.pose_landmarks.landmark[mp_holistic.PoseLandmark.LEFT_WRIST]
        elbow_left = results.pose_landmarks.landmark[mp_holistic.PoseLandmark.LEFT_ELBOW]
        
        # Convert normalized coordinates to pixel coordinates
        wrist_left_coords = (int(wrist_left.x * frame.shape[1]), int(wrist_left.y * frame.shape[0]))
        elbow_left_coords = (int(elbow_left.x * frame.shape[1]), int(elbow_left.y * frame.shape[0]))

        # Draw circles for wrist and elbow
        cv2.circle(frame, wrist_left_coords, 5, (255, 0, 0), -1)
        cv2.circle(frame, elbow_left_coords, 5, (255, 0, 0), -1)

        # Draw line to represent the left arm connection (wrist -> elbow)
        cv2.line(frame, wrist_left_coords, elbow_left_coords, (255, 0, 0), 2)

    # Display the resulting frame
    cv2.imshow('Left Hand and Arm (Up to Elbow) Tracking', frame)

    # Exit on pressing the 'q' key
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the video capture object and close OpenCV windows
cap.release()
cv2.destroyAllWindows()
