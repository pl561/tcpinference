import sys
import os
import socket
from .utils import receive_string, receive_image, send_string, send_image


class ImageTCPClient:
    """ 
    TCP socket client to send and receive images 
    """

    def __init__(self, addr, port, autoconnect=True, verbose=1):
        # client socket
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.addr = addr
        self.port = int(port)
        self.verbose = verbose

        if autoconnect:
            self.connect()

    def connect(self):
        self.s.connect((self.addr, self.port))
        if self.verbose > 0:
            print('[*] Client connected to {}:{}.'.format(self.addr, self.port))

    def close(self):
        self.s.close()
        if self.verbose > 0:
            print('[*] Client closed.')

    def receive_string(self):
        string = receive_string(self.s)
        return string
    
    def send_string(self, string):
        send_string(self.s, string)

    def receive_image(self):
        image = receive_image(self.s)
        return image
    
    def send_image(self, image, jpeg_quality=80):
        send_image(self.s, image, jpeg_quality=jpeg_quality)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()
