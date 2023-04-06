import socket
import time

print('Enter IP of ESP32')
UDP_IP = "192.168.1.100"
UDP_PORT = 5005
MESSAGE = b"Hello, World!"

while(True):
    print("UDP target IP: %s" % UDP_IP)
    print("UDP target port: %s"% UDP_PORT)
    print("message: %s"% MESSAGE)

    sock = socket.socket(socket.AF_INET,
                         socket.SOCK_DGRAM)
    sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
    time.sleep(2)


