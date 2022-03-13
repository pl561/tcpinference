"""
Module: 


"""
import os
import sys
import time
import argparse
import pathlib
# import _init_paths
import torch
import torchvision
from torch import nn
from PIL import Image
import cv2
import numpy as np
sys.path.insert(0, '/home/pascal/projects/segmentation/github/cvlibs')
# tcpsend(images, rescale=False, istensor=False, colormap=None)
from imagehandler import TensorImage, cat3
from tcpviewer import tcpsend
# from epath import EPath
from buffercom import TensorTCPServer, TensorTCPClient



def do(args):
    addr, port = '10.1.61.12', 12345
    if args.client:
        with TensorTCPClient(args.addr, args.port) as client:
            # a = client.receive_tensor_shape()

            for i in range(10):
                t = client.receive_float_tensor()
                client.send_float_tensor(t)
                print('received:\n', t)
    else:
        server = TensorTCPServer(args.addr, args.port)
        server.accept()
        t = np.random.rand(3, 2)
        print(t)
        for i in range(10):
            server.send_float_tensor(t)
            t = server.receive_float_tensor()
            print(t)

        server.close_client()
        server.close_server()



def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--client', action='store_true', help='activate client mode')
    
    parser.add_argument('--addr', type=str, default='10.1.61.12', help='ip addr')
    parser.add_argument('--port', type=int, default=12345, help='port no')
    args = parser.parse_args()
    
    
    
    
    
    
    
    do(args)


if __name__ == '__main__':
    main()
