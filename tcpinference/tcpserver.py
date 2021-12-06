import sys
import os
import socket
from .utils import send_string, receive_string, receive_image, send_image



class ImageTCPServer:
    def __init__(self, addr, port, autolisten=True, verbose=1):
        # client socket
        self.server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.addr = addr
        self.port = int(port)
        self.verbose = verbose

        if autolisten:
            self.listen()

    def listen(self):
        self.server_sock.bind((self.addr, self.port))
        self.server_sock.listen()
        if self.verbose > 0:
            print('[*] Listening on {}:{}'.format(self.addr, self.port))


    def accept(self):
        self.client_sock, self.client_addr_port = self.server_sock.accept()
        if self.verbose > 0:
            print('[*] client {}:{}'.format(*self.client_addr_port))

    def receive_string(self):
        string = receive_string(self.client_sock)
        return string
    
    def send_string(self, string):
        send_string(self.client_sock, string) 

    def receive_image(self):
        image = receive_image(self.client_sock)
        return image
    
    def send_image(self, image, jpeg_quality=50):
        send_image(self.client_sock, image, jpeg_quality=jpeg_quality) 

    def close_client(self):
        self.client_sock.close()
        if self.verbose > 0:
            print('[x] client {}:{}'.format(*self.client_addr_port))


    def close_server(self):
        self.server_sock.close()
        if self.verbose > 0:
            print('[x] server {}:{}'.format(*self.client_addr_port))
