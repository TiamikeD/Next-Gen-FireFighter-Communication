import cv2
import socket
import sys
import time
import pickle

if len(sys.argv) != 2:
    print("Usage: python server.py <client_ip>")
    sys.exit()



cap = cv2.VideoCapture(0)

UDP_IP = sys.argv[1]
UDP_PORT = 5005
encode_param = [int(cv2.IMWRITE_JPEG_QUALITY),35]

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Define the maximum packet size
MAX_PACKET_SIZE = 1436

while cap.isOpened():
    #time.sleep(1)
    # Capture frame-by-frame
    ret, frame = cap.read()

    if ret:
        # Encode the frame as a string of bytes in BMP format
        data = cv2.imencode('.jpg', frame, encode_param)[1].tobytes()
        #data = pickle.dumps(frame,0) 

        # Split the data into smaller chunks
        chunks = [data[i:i+MAX_PACKET_SIZE] for i in range(0, len(data), MAX_PACKET_SIZE)]
        # Send each chunk to the UDP receiver
        print("\n NEW FRAME")
        for chunk in chunks:
#            time.sleep(1)
            print(chunk)
            print(len(chunk))
            sock.sendto(chunk, (UDP_IP, UDP_PORT))

        # Display the resulting frame
#        cv2.imshow('frame', frame)

        # Press 'q' to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# Release everything
cap.release()
cv2.destroyAllWindows()
