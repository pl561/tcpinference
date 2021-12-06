"""
quick example

workflow: receives an image, sends a string, sends an image
"""

import os
import sys
import torch
import cv2
import numpy as np
from tcpclient import ImageTCPClient


def main():

    # model_deeplabv3 = torch.hub.load(
    #     'pytorch/vision:v0.6.0', 'deeplabv3_resnet101', pretrained=True).cuda()
    # model_deeplabv3.eval()

    kernel = np.ones((5,5),np.float32)/25


    ADDRESS, PORT = '123.123.123.123', 45612
    with ImageTCPClient(ADDRESS, PORT, autoconnect=True, verbose=1) as client:
        while True:
            # receives a uint8 color image as a numpy array
            image = client.receive_image()

            # preprocessing... (normalize, CHW, etc)
            # image_vis = model_deeplabv3(img)
            # postprocessing...


            # quick example
            image_vis = cv2.blur(image, -1, kernel)

            filename = 'hello.jpg'  # convenient to send filenames but it can contain anything
            client.send_string(filename)

            # expects uint8 HWC color image as a numpy array
            client.send_image(image_vis, jpeg_quality=70)  