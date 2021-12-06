"""
visualization server

"""

import os
import sys
import time
import argparse
import datetime
import cv2
from tcpserver import ImageTCPServer

FPS = 10.0

class Keys:
    f = ord('f')  # flip the image with a vertical axis
    q = ord('q')  # quit program
    s = ord('s')  # screenshot
    r = ord('r')  # video recording toggle key


def get_now():
    return datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')


def display_r(img, msg):
    """
    display a text msg indicating that the video stream is being recorded
    """
    font = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(img, msg, (200, 200), font, 1, (100, 255, 0), 3, cv2.LINE_AA)

def display_fps(img, fps):
    font = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(img, fps, (5, 25), font, 1, (100, 255, 0), 3, cv2.LINE_AA)


def run_server(args):
    """
    listen on local machine and wait for gpu server client to connect
    python sender.py LOCAL_IP LOCAL_PORT

    worflow: sends an image, receives a string, receives an image
    """

    server = ImageTCPServer(args.addr, args.port, autolisten=True, verbose=1)
    server.accept()

    flip = args.flip
    quality = 50
    w, h = 256, 256
    image_size = w, h
    cap = cv2.VideoCapture(0)
    is_recording = False


    font = cv2.FONT_HERSHEY_SIMPLEX
    prev_frame_time = 0
    new_frame_time = 0
    


    print('Webcam capture started')


    while (cap.isOpened()):
        # t0 = time.time()
        _, frame = cap.read()
        new_frame_time = time.time()
        fps = 1/(new_frame_time-prev_frame_time) 
        prev_frame_time = new_frame_time
        fps = '{:02d}'.format(int(fps))
        frame = cv2.resize(frame, image_size)
        try:
            server.send_image(frame, jpeg_quality=quality)
            img = server.receive_image()
        except Exception as e:
            print(e)
            server.close_client()
            cap.release()
            print('Webcam capture stopped')
            print('[E] client socket closed, waiting for a new client')
            
            server.accept()
            cap = cv2.VideoCapture(0)
            print('Webcam capture started')


        if flip:
            img = cv2.flip(img, 1)

        key = cv2.waitKey(1) & 0xFF

        if key == Keys.q:
            print('\n[q] bye bye                    ')
            break
        elif key == Keys.s:
            print('\n[s] Screenshot!                    ')
            fname = 'screenshots/screenshot_{}.jpg'.format(get_now())
            print('Screenshot:', fname)
            cv2.imwrite(fname, img)
        elif key == Keys.r:
            if is_recording:
                print('\n[r] Stop recording                    ')
                videowriter.release()
                is_recording = False
            else:
                print('\n[r] Start recording                    ')
                no_ext = 'videos/VID{}'.format(get_now())
                fname = no_ext + '.avi'
                fourcc = cv2.VideoWriter_fourcc(*'XVID')
                w, h = img.shape[:2]
                videowriter = cv2.VideoWriter(fname, fourcc, FPS, (h, w))
                is_recording = True

        display_fps(img, fps)
        if is_recording:
            result = videowriter.write(img)

            display_r(img, 'R')
        cv2.imshow('frame', img)


    cap.release()
    videowriter.release()
    cv2.destroyAllWindows()
    server.close_client()
    server.close_server()


def main():
    """ runs a TCP server to send and receive image through local network """

    parser = argparse.ArgumentParser()
    parser.add_argument('--flip', default=False, type=int,
                        help='horizontally flip the image if activated')
    
    parser.add_argument('addr', type=str, help='IP address')
    parser.add_argument('port', type=int, help='port number')
    args = parser.parse_args()


    run_server(args)


if __name__ == "__main__":
    main()