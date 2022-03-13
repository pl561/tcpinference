from functools import reduce
import sys
import cv2
import socket
import struct
import math
import zlib
import time
import numpy as np


size_long = struct.calcsize('l')  # 8 bytes
size_float = struct.calcsize('f') # 4 bytes


def receive_byte_array(sock, num_bytes, buff_size=4096):
    # 4096 # 4 KiB
    data = b''
    cnt = 0
    if num_bytes < buff_size:
        buff_size = num_bytes
    while True:
        part = sock.recv(buff_size)
        data += part
        print(len(data), num_bytes)
        if len(data) == num_bytes or len(part) == 0:
            break
        cnt += 1
    return data


# def send_byte_array(sock, data, flags=0):
#     ret = sock.sendall(data)
#     return ret


def send_byte_array(sock, data, flags=0):
    ret = sock.send(data, flags)
    if ret > 0:
        return send_byte_array(sock, data[ret:], flags)
    else:
        return None


def receive_float_array(sock, num_floats):
    buffer = receive_byte_array(sock, num_floats * size_float)
    # print(f'size of received buffer: {len(buffer)} bytes')
    float_array = struct.unpack(f'{num_floats}f', buffer)
    return float_array


def send_float_array(sock, float_array):
    byte_array = struct.pack('%sf' % len(float_array), *float_array)
    return send_byte_array(sock, byte_array, flags=0)


def receive_long_array(sock, num_longs):
    buffer = receive_byte_array(sock, num_longs * size_long)
    # print(f'size of received buffer: {len(buffer)} bytes')
    long_array = struct.unpack(f'{num_longs}l', buffer)
    return long_array


def send_long_array(sock, long_array):
    byte_array = struct.pack('%sl' % len(long_array), *long_array)
    return send_byte_array(sock, byte_array, flags=0)


def receive_string(sock):
    strlen = receive_long_array(sock, 1)[0]
    # print('receive_string:length_packed', strlen)

    data = receive_byte_array(sock, strlen)
    # print('receive_string:string', data)
    string = data.decode('utf-8')
    return string


def send_string(sock, string):
    send_long_array(sock, [len(string)])
    data = bytearray(string, 'utf-8')
    return send_byte_array(sock, data, flags=0)


def receive_tensor_shape(sock):
    shapelen = receive_long_array(sock, 1)[0]
    assert shapelen < 10
    shape = tuple(receive_long_array(sock, shapelen))
    # print(f'received shape: {shape}')
    return shape


def send_tensor_shape(sock, shape):
    return send_long_array(sock, (len(shape),) + tuple(shape))


def receive_float_tensor(sock):
    shape = receive_tensor_shape(sock)
    num_floats = reduce(lambda x, y: x * y, shape)
    float_array = receive_float_array(sock, num_floats)
    float_tensor = np.array(float_array).reshape(shape)
    return float_tensor


def send_float_tensor(sock, tensor):
    shape = tensor.shape
    float_array = tensor.flatten()
    send_tensor_shape(sock, shape)
    return send_float_array(sock, float_array)




class SocketBaseIO:
    client_sock = None
    def receive_byte_array(self, num_bytes, buff_size=4096):
        return receive_byte_array(self.client_sock, num_bytes, buff_size=buff_size)
    
    def send_byte_array(self, data, flags=0):
        return send_byte_array(self.client_sock, data, flags=flags)

    def receive_float_array(self, num_floats):
        return receive_float_array(self.client_sock, num_floats)
    
    def send_float_array(self, float_array):
        return send_float_array(self.client_sock, float_array)

    def receive_long_array(self, num_longs):
        return receive_float_array(self.client_sock, num_longs)
    
    def send_long_array(self, long_array):
        return send_float_array(self.client_sock, long_array)

    def receive_string(self):
        return receive_string(self.client_sock)
    
    def send_string(self, string):
        return send_string(self.client_sock, string)

    def receive_tensor_shape(self):
        return receive_tensor_shape(self.client_sock)
    
    def send_tensor_shape(self, shape):
        return send_tensor_shape(self.client_sock, shape)

    def receive_float_tensor(self):
        return receive_float_tensor(self.client_sock)
    
    def send_float_tensor(self, tensor):
        return send_float_tensor(self.client_sock, tensor)


class TensorTCPServer(SocketBaseIO):
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

    def close_client(self):
        self.client_sock.close()
        if self.verbose > 0:
            print('[x] client {}:{}'.format(*self.client_addr_port))

    def close_server(self):
        self.server_sock.close()
        if self.verbose > 0:
            print('[x] server {}:{}'.format(*self.client_addr_port))


class TensorTCPClient(SocketBaseIO):
    """
    TCP socket client
    """

    def __init__(self, addr, port, autoconnect=True, verbose=1):
        # client socket
        self.client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.addr = addr
        self.port = int(port)
        self.verbose = verbose

        if autoconnect:
            self.connect()

    def connect(self):
        self.client_sock.connect((self.addr, self.port))
        if self.verbose > 0:
            print('[*] Client connected to {}:{}.'.format(self.addr, self.port))

    def close(self):
        self.client_sock.close()
        if self.verbose > 0:
            print('[*] Client closed.')

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()