import cv2
import numpy as np
import easyocr

# The URL from your ESP32-CAM Serial Monitor (e.g., /stream or /cam-hi.jpg)
url = "http://100.87.96.161:81/stream" 
cap = cv2.VideoCapture(url)
frame=np.zeros(1)
for i in range(2):
    ret, frame = cap.read()
    if not ret:
        break
    # --- PERFORM CV ON 'frame' HERE ---
    # Example: gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # cv2.imshow("ESP32-CAM Feed", frame)

    # break
    if cv2.waitKey(1) == 27: # Press ESC to exit
        break
cv2.imwrite("hi.jpg",frame)

cap.release()
