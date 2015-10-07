#! /usr/bin/env python
import argparse
import cv
import cv2
import sys
import os
import pytesseract
from PIL import Image, ImageEnhance
def rotate_img(img, angle):
    rot = img.convert('RGBA').rotate(angle, resample=Image.BILINEAR, expand=True)
    fff = Image.new('RGBA', rot.size, (255,)*4)
    out = Image.composite(rot, fff, rot)
    return out


def filter_image(img, threshold=256):
    w,h = img.size
    for x in range(w):
        for y in range(h):
            if img.mode=="RGB":
                r, g, b = img.getpixel((x,y))
                if r+g+b > threshold:
                    img.putpixel((x,y), (255,255,255))

                #else:
                #   img.putpixel((x,y), (0,0,0))
            elif img.mode=="RGBA":
                r, g, b,a = img.getpixel((x,y))
                if r+g+b > threshold:
                    img.putpixel((x,y), (255,255,255,255))
                #else:
                #   img.putpixel((x,y), (0,0,0,255))

import time
import socket
import enchant
def send_UDP(word):
    UDP_IP = "127.0.0.1"
    UDP_PORT = 12345
    message = word
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (UDP_IP, UDP_PORT))
dictionary = enchant.Dict('en_US')
def demo_word_ocr_cv(img, point, width, height):
    w,h,_ = img.shape
    x,y = point
    cropped_img = img[y-height:y, x-width/2:x+width/2]
    img_path = str(time.time())+".png"
    cv2.imwrite(img_path, cropped_img)
    try:
        new_img = Image.open(img_path)
    except Exception as e:
        print str(e)
        os.remove(img_path)
        return False
    enh = ImageEnhance.Contrast(new_img)
    new_img = enh.enhance(1.9)
    filter_image(new_img, threshold=600)
    word0 = pytesseract.image_to_string(rotate_img(new_img,0), lang='eng')
    if len(word0)>0 and dictionary.check(word0):
        print word0
        send_UDP(word0)
        #new_img.close()
        os.remove(img_path)
        return True
    # new_img.close()
    os.remove(img_path)
    return False


#image_cv2 = cv2.imread("./kakao.jpeg")
#print demo_word_ocr_cv(image_cv2, (1262, 926), 120, 45)






class LaserTracker(object):

    def __init__(self, cam_width=640, cam_height=480, hue_min=20, hue_max=160,
                 sat_min=100, sat_max=255, val_min=200, val_max=256,
                 display_thresholds=False):
        """
        * ``cam_width`` x ``cam_height`` -- This should be the size of the
        image coming from the camera. Default is 640x480.

        HSV color space Threshold values for a RED laser pointer are determined
        by:

        * ``hue_min``, ``hue_max`` -- Min/Max allowed Hue values
        * ``sat_min``, ``sat_max`` -- Min/Max allowed Saturation values
        * ``val_min``, ``val_max`` -- Min/Max allowed pixel values

        If the dot from the laser pointer doesn't fall within these values, it
        will be ignored.

        * ``display_thresholds`` -- if True, additional windows will display
          values for threshold image channels.

        """

        self.cam_width = cam_width
        self.cam_height = cam_height
        self.hue_min = hue_min
        self.hue_max = hue_max
        self.sat_min = sat_min
        self.sat_max = sat_max
        self.val_min = val_min
        self.val_max = val_max
        self.display_thresholds = display_thresholds

        self.capture = None  # camera capture device
        self.channels = {
            'hue': None,
            'saturation': None,
            'value': None,
            'laser': None,
        }

    def create_and_position_window(self, name, xpos, ypos):
        """Creates a named widow placing it on the screen at (xpos, ypos)."""
        # Create a window
        cv2.namedWindow(name, cv2.CV_WINDOW_AUTOSIZE)
        # Resize it to the size of the camera image
        cv2.resizeWindow(name, self.cam_width, self.cam_height)
        # Move to (xpos,ypos) on the screen
        cv2.moveWindow(name, xpos, ypos)

    def setup_camera_capture(self, device_num=1):
        """Perform camera setup for the device number (default device = 0).
        Returns a reference to the camera Capture object.

        """
        try:
            device = int(device_num)
            sys.stdout.write("Using Camera Device: {0}\n".format(device))
        except (IndexError, ValueError):
            # assume we want the 1st device
            device = 0
            sys.stderr.write("Invalid Device. Using default device 0\n")

        # Try to start capturing frames
        self.capture = cv2.VideoCapture(device)
        if not self.capture.isOpened():
            sys.stderr.write("Faled to Open Capture device. Quitting.\n")
            sys.exit(1)

        # set the wanted image size from the camera
        self.capture.set(
            cv.CV_CAP_PROP_FRAME_WIDTH,
            self.cam_width
        )
        self.capture.set(
            cv.CV_CAP_PROP_FRAME_HEIGHT,
            self.cam_height
        )
        return self.capture

    def handle_quit(self, delay=10):
        """Quit the program if the user presses "Esc" or "q"."""
        key = cv2.waitKey(delay)
        c = chr(key & 255)
        if c in ['q', 'Q', chr(27)]:
            sys.exit(0)

    def threshold_image(self, channel):
        if channel == "hue":
            minimum = self.hue_min
            maximum = self.hue_max
        elif channel == "saturation":
            minimum = self.sat_min
            maximum = self.sat_max
        elif channel == "value":
            minimum = self.val_min
            maximum = self.val_max

        (t, tmp) = cv2.threshold(
            self.channels[channel], # src
            maximum, # threshold value
            0, # we dont care because of the selected type
            cv2.THRESH_TOZERO_INV #t type
        )

        (t, self.channels[channel]) = cv2.threshold(
            tmp, # src
            minimum, # threshold value
            255, # maxvalue
            cv2.THRESH_BINARY # type
        )

        if channel == 'hue':
            # only works for filtering red color because the range for the hue is split
            self.channels['hue'] = cv2.bitwise_not(self.channels['hue'])


    def detect(self, frame):
        hsv_img = cv2.cvtColor(frame, cv.CV_BGR2HSV)

        # split the video frame into color channels
        h, s, v = cv2.split(hsv_img)
        self.channels['hue'] = h
        self.channels['saturation'] = s
        self.channels['value'] = v

        # Threshold ranges of HSV components; storing the results in place
        self.threshold_image("hue")
        self.threshold_image("saturation")
        self.threshold_image("value")

        # Perform an AND on HSV components to identify the laser!
        self.channels['laser'] = cv2.bitwise_and(
            self.channels['hue'],
            self.channels['value']
        )
        self.channels['laser'] = cv2.bitwise_and(
            self.channels['saturation'],
            self.channels['laser']
        )

        # Merge the HSV components back together.
        hsv_image = cv2.merge([
            self.channels['hue'],
            self.channels['saturation'],
            self.channels['value'],
        ])

        return hsv_image

    def display(self, img, frame):
        """Display the combined image and (optionally) all other image channels
        NOTE: default color space in OpenCV is BGR.
        """
        #(cnts, _) = cv2.findContours(self.new_frame.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        #for c in cnts:
        #    (x, y, w, h) = cv2.boundingRect(c)
        #    cv2.rectangle(self.new_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        (cnts, _) = cv2.findContours(self.channels['laser'].copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for c in cnts:
            a = cv2.contourArea(c)
            if a > 0 and a < 1200:
                (x, y, w, h) = cv2.boundingRect(c)
                #print '(%d, %d), %d' % (x+w/2, y+h/2, a)
                ret = demo_word_ocr_cv(frame, (x+w/2, y+h/2), 70, 20)
                if ret:
                    time.sleep(1.5)
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        cv2.imshow('RGB_VideoFrame', frame)
        cv2.imshow('LaserPointer', self.channels['laser'])
        #cv2.imshow('MotionTracking', self.new_frame)

        if self.display_thresholds:
            cv2.imshow('Thresholded_HSV_Image', img)
            cv2.imshow('Hue', self.channels['hue'])
            cv2.imshow('Saturation', self.channels['saturation'])
            cv2.imshow('Value', self.channels['value'])

    def setup_windows(self):
        sys.stdout.write("Using OpenCV version: {0}\n".format(cv2.__version__))

        # create output windows
        self.create_and_position_window('LaserPointer', 0, 0)
        self.create_and_position_window('RGB_VideoFrame', 10 + self.cam_width, 0)
        #self.create_and_position_window('MotionTracking', 20 + self.cam_width, 0)
        if self.display_thresholds:
            self.create_and_position_window('Thresholded_HSV_Image', 10, 10)
            self.create_and_position_window('Hue', 20, 20)
            self.create_and_position_window('Saturation', 30, 30)
            self.create_and_position_window('Value', 40, 40)


    def run(self):
        # Set up window positions
        self.setup_windows()
        # Set up the camera capture
        self.setup_camera_capture()
        #success, frame = self.capture.read()
        #self.prev = cv2.GaussianBlur(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY), (21, 21), 0)
        while True:
            # 1. capture the current image
            success, frame = self.capture.read()
            if not success: # no image captured... end the processing
                sys.stderr.write("Could not read camera frame. Quitting\n")
                sys.exit(1)
            #new_frame = cv2.GaussianBlur(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY), (21, 21), 0)
            #self.new_frame = cv2.dilate(cv2.threshold(cv2.absdiff(new_frame, self.prev), 25, 255, cv2.THRESH_BINARY)[1], None, iterations=2)
            #self.prev = new_frame
            hsv_image = self.detect(frame)
            self.display(hsv_image, frame)
            self.handle_quit()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run the Laser Tracker')
    parser.add_argument('-W', '--width',
        default=640,
        type=int,
        help='Camera Width'
    )
    parser.add_argument('-H', '--height',
        default=480,
        type=int,
        help='Camera Height'
    )
    parser.add_argument('-u', '--huemin',
        default=20,
        type=int,
        help='Hue Minimum Threshold'
    )
    parser.add_argument('-U', '--huemax',
        default=160,
        type=int,
        help='Hue Maximum Threshold'
    )
    parser.add_argument('-s', '--satmin',
        default=100,
        type=int,
        help='Saturation Minimum Threshold'
    )
    parser.add_argument('-S', '--satmax',
        default=255,
        type=int,
        help='Saturation Maximum Threshold'
    )
    parser.add_argument('-v', '--valmin',
        default=200,
        type=int,
        help='Value Minimum Threshold'
    )
    parser.add_argument('-V', '--valmax',
        default=255,
        type=int,
        help='Value Maximum Threshold'
    )
    parser.add_argument('-d', '--display',
        action='store_true',
        help='Display Threshold Windows'
    )
    params = parser.parse_args()

    tracker = LaserTracker(
        cam_width=params.width,
        cam_height=params.height,
        hue_min=params.huemin,
        hue_max=params.huemax,
        sat_min=params.satmin,
        sat_max=params.satmax,
        val_min=params.valmin,
        val_max=params.valmax,
        display_thresholds=params.display
    )
    tracker.run()
