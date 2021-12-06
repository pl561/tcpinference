# Description

This module allows to perform model inference (or any image processing tasks) on a GPU server while visualizing the live results on a computer desktop or laptop with a camera. Any computer with Python + OpenCV can run the TCP server. 


Context:

When a TCP client (eg: GPU server side), connects to the server (laptop), its camera will start and the server will send a webcam frame to the client.

The client performs image processing (eg: model inference such as classification, detection, pose and segmentation) tasks on the received image and sends (see workflow) the result image to the laptop server which displays it in real time.



| Hardware                                 | TCP architecture  | Python script           |
|------------------------------------------|-------------------|-------------------------|
| Laptop/Desktop computer side with camera | local server      | visualization_server.py |
| GPU server                               | local client      | client_example.py       |



# Installation procedure


installation procedure...



# Workflow

## Laptop/Desktop

1. sends an image
2. receives a string
3. receives an image

## GPU server 

1. receives an image
2. sends a string
3. sends an image


# Quick usage

## Laptop/Desktop side 1/2

run the server:
```bash
# local IP of the laptop + local port
python visualization_server.py 123.123.123.123 65489
```


## GPU server side 2/2

Set up the TCP client

```python
import cv2
from tcpinference import ImageTCPClient

ADD = '123.123.123.123'
PORT = 65489
with ImageTCPCLient(ADD, PORT, autoconnect=True, verbose=1) as client:
    while True:
        # receive the image from the server
        image = client.receive_image()
        # your processing on the received image

        # for example flip horizontally the image
        modified_image = cv2.flip(image, 1)
        # send it back to the server
        client.send_image(modified_image)
```


