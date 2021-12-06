"""
visualization server

"""

import os
import sys
import time
import argparse
import datetime
import cv2
from tcpclient import ImageTCPClient

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


def run_client(client):
    cap = cv2.VideoCapture(0)
    quality = 50
    w, h = 256, 256
    image_size = w, h
    cap = cv2.VideoCapture(0)
    is_recording = False

    font = cv2.FONT_HERSHEY_SIMPLEX
    prev_frame_time = 0
    new_frame_time = 0

    print('Webcam capture started')

    c = 0
    while (cap.isOpened()):
        # t0 = time.time()
        print('reading cap', c)
        _, frame = cap.read()
        print('cap obtained', c)
        new_frame_time = time.time()
        fps = 1 / (new_frame_time - prev_frame_time)
        prev_frame_time = new_frame_time
        fps = '{:02d}'.format(int(fps))
        frame = cv2.resize(frame, image_size)
        img = frame
        print('sending img', c)
        client.send_image(img, quality)
        print('image sent', c)
        _ = client.receive_string()
        c += 1
        # key = cv2.waitKey(1) & 0xFF

        # if key == Keys.s:
        #     print('\n[s] Screenshot!                    ')
        #     fname = 'screenshots/screenshot_{}.jpg'.format(get_now())
        #     print('Screenshot:', fname)
        #     cv2.imwrite(fname, img)
        # elif key == Keys.r:
        #     if is_recording:
        #         print('\n[r] Stop recording                    ')
        #         videowriter.release()
        #         is_recording = False
        #     else:
        #         print('\n[r] Start recording                    ')
        #         no_ext = 'videos/VID{}'.format(get_now())
        #         fname = no_ext + '.avi'
        #         fourcc = cv2.VideoWriter_fourcc(*'XVID')
        #         w, h = img.shape[:2]
        #         videowriter = cv2.VideoWriter(fname, fourcc, FPS, (h, w))
        #         is_recording = True
        #
        # display_fps(img, fps)
        # if is_recording:
        #     result = videowriter.write(img)
        #     display_r(img, 'R')
        # cv2.imshow('frame', img)

    cap.release()
    # videowriter.release()
    # cv2.destroyAllWindows()


def main():
    client = ImageTCPClient('0.0.0.0', 60000,  autoconnect=True, verbose=1)
    run_client(client)



if __name__ == "__main__":
    main()