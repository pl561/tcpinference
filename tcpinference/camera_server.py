"""
quick example

workflow: receives an image, sends a string, sends an image
"""

import os
import sys
#import torch
import cv2
import numpy as np
from tcpserver import ImageTCPServer

# TODO: abstract port
# TODO: client reuseable
# TODO: client multiple

ADDRESS, PORT = '0.0.0.0', 60000
# this is server of the cemera
def main():

    server = ImageTCPServer(ADDRESS, PORT, autolisten=True, verbose=1)
    server.accept()

    while True:
        img = server.receive_image()
        print('-------------\n', img)
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            print('\n[q] bye bye                    ')
            break
        print('display img')
        cv2.imshow('frame', img)
        print("img showed")
        server.send_string('0')
        # try:
        #     img = server.receive_image()
        #     print('-------------', img)
        #     key = cv2.waitKey(1) & 0xFF
        #     if key == ord('q'):
        #         print('\n[q] bye bye                    ')
        #         break
        #     cv2.imshow('frame', img)
        # except Exception as e:
        #     print(e)
        #     server.close_client()
        #     print('[E] server socket error, waiting for a new client')

    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
