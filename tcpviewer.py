"""
Image viewer (client)

send images from a server without GUI to a computer with GUI
usages: commandline, inside Python code

for commandline: create bash alias, eg: 
alias tcpviewer="${python_bin} {path_to_package}/tcpinference/tcpviewer.py"
"""

import sys
import os
import argparse
import cv2
from PIL import Image
import numpy as np
from tcpinference.tcpclient import ImageTCPClient


default_addr, default_port = 'computer_gui_ip', 12345

# for convenience
default_net_conf = ":".join([default_addr, str(default_port)])





def normalize(image, a=0, b=255):
    mini = image.min()
    maxi = image.max()
    if mini == maxi:
        # image with only one color for every pixel
        if maxi != 0:
            return image/maxi*b
        else:
            return image
    # assert mini != maxi, 'image only contain the same value'
    ret = (image - mini) / (maxi - mini) * (b - a) + a
    return ret


def get_binary_mask(mask):
    """
        given a mask
        returns: the binary mask of non zero values
    """
    binary_mask = np.zeros_like(mask)
    binary_mask[mask != 0] = 1
    return binary_mask


def tcpsend(image,
            network_config=default_net_conf,
            istensor=False,
            rescale=False,
            colormap=None,
            # colormap=cv2.COLORMAP_INFERNO,
            ncols=1,
            size=None,
            fname=None):
    """
        colormap flags
        https://docs.opencv.org/4.5.2/d3/d50/group__imgproc__colormap.html
        deactivate colormap flag by setting None when visualizing images and not conv filters for example
    """
    if isinstance(network_config, tuple) or isinstance(network_config, list):
        addr, port = network_config
    elif isinstance(network_config, str):
        # config of the form addr:port
        addr, port = network_config.split(':')
        port = int(port)

    if fname is None:
        imagepath = 'tcpsend.png'
    else:
        imagepath = fname
    quality = 90

    if rescale:
        image = normalize(image)
    # process the tensor here
    if istensor:
        images = []
        for i in range(image.shape[0]):
            f = image[i]  # filter/ slice of the tensor
            if colormap is not None:
                f = cv2.applyColorMap(f.astype(np.uint8), colormap)
            if size is not None:
                f = cv2.resize(f, size, interpolation=cv2.INTER_NEAREST)
            images.append(f)

        
        vsep = np.zeros((images[0].shape[1], 1, 3)) + 255
        hsep_w = (size[1] + vsep.shape[1]) * ncols
        # times num cols + offset width vsep
        hsep = np.zeros((1, hsep_w, 3)) + 255

        if ncols == 0:
            image_concat = np.concatenate(images, axis=1)
        else:
            black = np.zeros_like(images[0])
            # pad the list with black images
            while len(images) % ncols != 0:
                images.append(black)
            stack = []
            rows = []
            for image in images:
                stack.append(image)
                stack.append(vsep)

                if len(stack) == 2 * ncols:

                    row = np.concatenate(stack, axis=1)
                    # row = np.concatenate(hsep, axis=1)
                    rows.append(row)
                    rows.append(hsep)
                    stack = []
            image_concat = np.concatenate(rows, axis=0)
            image = image_concat  # image to send

    else:

        if colormap is not None:
            image = cv2.applyColorMap(image.astype(np.uint8), colormap)

        if size is not None:
            assert isinstance(size, tuple) or isinstance(size, list)
            assert 1 < len(size) < 3
            # nearest interpolation for mask and filters
            image = cv2.resize(image, size, interpolation=cv2.INTER_NEAREST)

    with ImageTCPClient(addr, port, autoconnect=True, verbose=1) as client:
        print('[*] Sending image path: {}'.format(imagepath))
        client.send_string(imagepath)

        print('[*] Sending image')
        client.send_image(image, jpeg_quality=quality)
        # client.receive_string()


def tcp_image_sender(args):
    """
    send function used for the command line tool
    """

    flip = args.flip
    quality = 80

    # imagepath = args.image_input
    images = []
    num_page = args.page

    if args.flist:
        print('Reading flist...')
        flist_fname = args.image_input[0]
        with open(flist_fname, 'r') as fd:
            fnames = []
            index = 0
            while True:
                index += 1
                line = fd.readline().strip()
                fnames.append(line)
                if not line:
                    break
                p = f'{index}                   \r'
                print(p, end='')

    else:
        fnames = args.image_input
    
    if args.max != -1:
        fnames_bypage = fnames[(num_page - 1) * args.max: num_page * args.max]

    print(f'Total number of images: {len(fnames)}. Found {len(fnames_bypage)} input images in page {args.page}')
    index = 0
    for imagepath in fnames_bypage:
        if os.path.exists(imagepath):
            bsn = os.path.basename(imagepath)
            image = np.array(Image.open(imagepath))
            msg = f'{index}/{len(fnames_bypage)} {bsn} {image.shape}  \r'
            print(msg)
            # image = cv2.imread(imagepath)
            if len(image.shape) == 3:
                image = image[:, :, ::-1]
            values = np.unique(image)

            if True in values or False in values:
                image = image.astype(np.uint8)

            if len(image.shape) == 3 and image.shape[-1] == 4:
                image = image[:, :, :3]  # for rgba image format
            
            if args.printshape:
                print(bsn, image.shape)
            if len(image.shape) == 2:
                image = np.dstack((image,) * 3)
            if args.flipcolors:
                image = image[:, :, ::-1]
            
            # only values
            if args.only != '':
                only_values = map(int, args.only.split(',')[:2])
                for v in only_values:
                    image[image == v] = 1
                image[image != 1] = 0
            if args.binarizemask:
                # we assume masks are png images and jpg images are not
                if imagepath.endswith('.png'):
                    image = get_binary_mask(image)
            if args.rescale:
                bsn = os.path.basename(imagepath)
                values = np.unique(image)
                print(f'unique {bsn}: {values}')

                image = normalize(image)

        else:
            print('[Not found] {}'.format(imagepath))
            sys.exit()
        index += 1

        if args.max != -1 and len(images) == args.max:
            break

        if args.size != '':
            w, h = map(int, args.size.split(',')[:2])
            image = cv2.resize(image, (h, w))
        


        if args.threshold is not None:
            # th = int(args.threshold)
            # assert 0 <= th <= 256, 'threshold not in RGB range'
            img_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

            th = int(np.mean(img_gray))


            img_gray[img_gray <= th] = 0
            img_gray[img_gray > th] = 255
            # kernel = np.ones((5, 5), np.uint8)
            # img_gray = cv2.erode(img_gray, kernel, iterations=3)
            # img_gray = cv2.dilate(img_gray, kernel, iterations=1)
            image = img_gray
        images.append(image)


    if args.ncols == 0:
        image_concat = np.concatenate(images, axis=1)
    else:
        black = np.zeros_like(images[0])
        # pad the list with black images
        while len(images) % args.ncols != 0:
            images.append(black)
        stack = []
        rows = []
        for image in images:
            stack.append(image)
            if len(stack) == args.ncols:
                row = np.concatenate(stack, axis=1)
                rows.append(row)
                stack = []
        image_concat = np.concatenate(rows, axis=0)


    if not args.dontsend:
        with ImageTCPClient(args.addr, args.port, autoconnect=True, verbose=1) as client:
            if args.filename != '':
                imagepath = args.filename
            imagepath = os.path.basename(imagepath)
            print('[*] Sending image path: {}'.format(imagepath))
            client.send_string(imagepath)

            print('[*] Sending image')
            client.send_image(image_concat, jpeg_quality=quality)

    if args.savelocally:
        fname = imagepath
        if args.filename != '':
            fname = args.filename
        print(f'Saving locally: {fname}')
        cv2.imwrite(fname, image_concat)



def main():
    """
    runs a TCP server to receive image through local network
    simple image viewer with one image at a time
    """

    parser = argparse.ArgumentParser()
    parser.add_argument('--flip', default=False, type=int,
                        help='horizontally flip the image if activated')
    
    parser.add_argument('image_input', type=str, nargs='+', help='Input image to display')
    parser.add_argument('--size', type=str, default='', help='image size \'W,H\'')
    # parser.add_argument('--filename', type=str, default='', help='image file name')
    parser.add_argument('--addr', type=str, default=default_addr, help='IP address')
    parser.add_argument('--port', type=int, default=default_port, help='port number')
    parser.add_argument('--flipcolors', action='store_true', help='flips the color channels')
    parser.add_argument(
        '--flist', action='store_true',
        help='flist mode, image_input argument is a text file containing image full paths'
    )
    parser.add_argument('--rescale', action='store_true', help='rescale pixels to [0, 255]')
    parser.add_argument('--savelocally', action='store_true', help='saves the image in the local dir')
    parser.add_argument('--dontsend', action='store_true', help='dont send the images through tcp')
    parser.add_argument('--printshape', action='store_true', help='print each image shape')
    parser.add_argument('--filename', default='', help='image filename')
    parser.add_argument('--binarizemask', action='store_true', help='input images are considered as masks, send to display their binary mask version')
    parser.add_argument('--ncols', default=0, type=int, help='display only ncol columns per row of displayed images')
    parser.add_argument('--max', default=1, type=int, help='maximum number of images to display in one page')
    parser.add_argument('--page', default=1, type=int, help='number of pages given the maximum number of images per page to display')
    parser.add_argument('--only', default='', type=str, help='(comma separated integers) values to keep in the mask, other valued are zeroed')


    parser.add_argument(
        '-t', '--threshold', default=None,
        help='threshold the image to create a mask'
    )

    args = parser.parse_args()
    print(args)
    tcp_image_sender(args)


if __name__ == '__main__':
    main()
