"""
image receiver server
used with tcpviewer.py image sender client
"""

import os
import sys
import time
import argparse
import traceback
import cv2
import webbrowser
from tcpinference.tcpserver import ImageTCPServer




class Keys:
    f = ord('f')
    q = ord('q')
    left_arrow = 2424832
    right_arrow = 2555904
    up_arrow = 2490368
    down_arrow = 2621440



def tcp_image_receiver_server(args):
    """
    listen on local machine and wait for gpu server client to connect
    python sender.py 10.1.51.89 12345

    """

    server = ImageTCPServer(args.addr, args.port, autolisten=True, verbose=1)
    server.accept()

    flip = args.flip
    quality = 50
    w, h = 256, 256
    image_size = w, h
    resize_image = False

    while True:
        try:
            # print('[*] Waiting for image path')
            image_path = server.receive_string()
            image = server.receive_image()
            print('[*] Received image: {}'.format(image_path))
            # server.send_string('ok')
            # print('ok')
            # print('[*] Received image')
            bsn = os.path.basename(image_path)
            fname = os.path.join(args.directory, bsn)
            
            cv2.imwrite(fname, image)
            url = 'file://' + os.path.realpath(fname)
            webbrowser.open(url, new=2)


        except Exception as e:
            print(traceback.format_exception(*sys.exc_info()))
            server.close_client()
            # print('[E] client socket closed, waiting for a new client')
            server.accept()

    server.close_client()
    server.close_server()


def main():
    """ runs a TCP server to receive image through local network """

    default_addr, default_port = 'xxx.xxx.xxx.xxx', 12345
    default_dir = 'images/'
    if not os.path.exists(default_dir):
        os.mkdir(default_dir)

    parser = argparse.ArgumentParser()
    parser.add_argument('--flip', action='store_true',
                        help='horizontally flip the image if activated')
    
    parser.add_argument('--directory', type=str, default=default_dir, help='Directory to store received images')
    parser.add_argument('--addr', type=str, default=default_addr, help='IP address')
    parser.add_argument('--port', type=int, default=default_port, help='port number')
    args = parser.parse_args()


    tcp_image_receiver_server(args)


if __name__ == "__main__":
    main()