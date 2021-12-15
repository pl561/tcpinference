# Description

This repository contains tools to send images through TCP. Solves the problem of 

# Installation procedure


installation procedure...


# Usage


### Image viewer

## GUI side

run server
```
python image_receiver_server.py --addr xxx.xxx.xxx.xxx --port 12345
```


### non-GUI side

send image with command line
```
python tcpviewer.py image.jpg --size 1024,512 --addr xxx.xxx.xxx.xxx --port 12345
```

send image inside Python code (useful for visualization, debug, etc)
```python
from tcpviewer import tcpsend
tcpsend(image)
```



## Camera

Send image by image to simulate real-time video

### GUI side

run the server:
```bash
# local IP of the laptop + local port
python visualization_server.py 123.123.123.123 65489
```


### non-GUI side

TCP client

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



