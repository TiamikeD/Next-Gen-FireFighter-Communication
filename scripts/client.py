import cv2
import socket
import numpy as np
import sys
from threading import Thread
import pickle


#if len(sys.argv) != 2:
#    print("Usage: python client.py <client_ip>")
#    sys.exit()


#UDP_IP = sys.argv[1]
UDP_IP = socket.gethostbyname(socket.gethostname())
                            

class UdpVideoCapture():
    def __init__(self, udp_ip, udp_port):
        #super().__init__()
        self.udp_ip = udp_ip
        self.udp_port = udp_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', udp_port))

        self.max_packet_size = 1436
        self.frame_chunks = []
        self.frame = np.zeros((480, 640, 3), dtype=np.uint8)
        self.NEWFRAME = False


    def run(self):
        while True:
            #print(2)
            # Receive a chunk of data
            data, addr = self.sock.recvfrom(self.max_packet_size)

 #           print(1)
            # Decode the data into a numpy array
            print(data)
            print(len(data))
            chunk = np.frombuffer(data, dtype=np.uint8)
            #print(chunk)
            # Append the chunk to the list of frame chunks
            self.frame_chunks.append(chunk)

            # If we have received a full frame, decode it and return it
            if len(chunk) < self.max_packet_size:
                # Concatenate the frame chunks into a single numpy array
                frame_data = np.concatenate(self.frame_chunks, axis=None)
                #print(frame_data)
                #frame_inter = pickle.loads(frame_data, fix_imports=True, encoding="bytes")

                # Decode the frame data into a numpy array
                self.frame = cv2.imdecode(frame_data, cv2.IMREAD_COLOR)

                # Reset the frame chunks list
                self.frame_chunks = []

                self.NEWFRAME = True
                # Return the frame

                self.print_frame(True, self.frame)


    def release(self):
        self.sock.close()

    def print_frame(self, ret, frame):
        if ret:
            try:
                print(frame)
                cv2.imshow('frame', frame)
            except Exception as ex:
                print(ex)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                pass


udp_capture = UdpVideoCapture(UDP_IP, 5050)
udp_capture.run()


#udp_capture.release()
cv2.destroyAllWindows()
