"""
Module for common functions used for client and server

"""

import sys
import os
import math
import argparse

import cv2
import numpy as np
import socket
import struct
import zlib



size_long = struct.calcsize('l')

def recvall(sock, length, buff_size=4096):
    # 4096 # 4 KiB
    data = b''
    cnt = 0
    if length < buff_size:
        buff_size = length
    while True:
        part = sock.recv(buff_size)
        data += part
        if len(data) == length:
            break
        cnt += 1
    return data


def receive_string(socket):
    """
    receives a string from the tcp socket
    """
    # receives the length of the string first
    length_packed = socket.recv(size_long)
    length = struct.unpack('l', length_packed)[0]

    # receives the path with expected length
    data = recvall(socket, length)
    string = zlib.decompress(data).decode('utf-8')
    return string


def send_string(socket, string):
    """
    send a string over the TCP socket by specifying the string binary length
    """
    
    data = bytearray(string, 'utf-8')
    compressed = zlib.compress(data)
    length = struct.pack('l', len(compressed))
    socket.sendall(length)
    socket.sendall(compressed)


def receive_image(socket):
    """
    receives an image from the tcp socket s
    """
    # receives the length of the image first
    length_packed = socket.recv(size_long)
    length = struct.unpack('l', length_packed)[0]

    # receives the image with expected length
    data = recvall(socket, length)
    decompressed = zlib.decompress(data)
    img = cv2.imdecode(np.frombuffer(decompressed, dtype=np.uint8), 1)
    return img


def send_image(socket, image, jpeg_quality=50):
    """
    send an image over the TCP socket by specifying the imge binary length
    """
    encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), jpeg_quality]
    compress_img = cv2.imencode('.jpg', image, encode_param)[1]
    data = compress_img.tobytes()
    compressed = zlib.compress(data)
    length = struct.pack('l', len(compressed))
    socket.sendall(length)
    socket.sendall(compressed)
