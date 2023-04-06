import cv2
import socket
import sys
import time

if len(sys.argv) != 2:
    print("Usage: python server.py <client_ip>")
    sys.exit()



cap = cv2.VideoCapture(0)

UDP_IP = sys.argv[1]
UDP_PORT = 5005

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Define the maximum packet size
MAX_PACKET_SIZE = 1472

while cap.isOpened():
#    time.sleep(5)
    # Capture frame-by-frame
    ret, frame = cap.read()

    if ret:
        # Encode the frame as a string of bytes in BMP format
        data = cv2.imencode('.bmp', frame)[1].tobytes()

        # Split the data into smaller chunks
        chunks = [data[i:i+MAX_PACKET_SIZE] for i in range(0, len(data), MAX_PACKET_SIZE)]

        # Send each chunk to the UDP receiver
        for chunk in chunks:
#            time.sleep(1)
            sock.sendto(chunk, (UDP_IP, UDP_PORT))

        # Display the resulting frame
#        cv2.imshow('frame', frame)

        # Press 'q' to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# Release everything
cap.release()
cv2.destroyAllWindows()
