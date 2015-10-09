from camera import Camera
import cv, cv2

class LaserTracker:
    def __init__(self, hue_min, hue_max, sat_min, sat_max, val_min, val_max, func = None):
        """
        * Laser Tracker class. Filter the image by given conditions.
        * Currently, 20, 160, 100, 255, 200, 255 seems good.
        * If laser detected, call Func(x, y, w, h),
             which is given for the last argument
        """
        self.hue_min = hue_min
        self.hue_max = hue_max
        self.sat_min = sat_min
        self.sat_max = sat_max
        self.val_min = val_min
        self.val_max = val_max
        self.func = func

    def setFunc(self, func):
        self.func = func

    def getFilteredImage(self, image):
        """
        * Internal function of run()

        * Filter given image
        * The pixels in laser part have value 255.
        * Otherwise, the pixels have value 0.
        """
        hsv_image = cv2.cvtColor(image, cv.CV_BGR2HSV)

        h, s, v = cv2.split(hsv_image)

        _, tmp = cv2.threshold(h, self.hue_max, 0, cv2.THRESH_TOZERO_INV)
        _, tmp = cv2.threshold(tmp, self.hue_min, 255, cv2.THRESH_BINARY)
        hue_th = cv2.bitwise_not(tmp)

        _, tmp = cv2.threshold(s, self.sat_max, 0, cv2.THRESH_TOZERO_INV)
        _, tmp = cv2.threshold(tmp, self.sat_min, 255, cv2.THRESH_BINARY)
        sat_th = tmp

        _, tmp = cv2.threshold(v, self.val_max, 0, cv2.THRESH_TOZERO_INV)
        _, tmp = cv2.threshold(tmp, self.val_min, 255, cv2.THRESH_BINARY)
        val_th = tmp

        tmp = cv2.bitwise_and (hue_th, sat_th)
        laser_img = cv2.bitwise_and (tmp, val_th)

        return laser_img

    def run(self, image):
        if self.func == None:
            return None

        #(cnts, _) = cv2.findContours(self.new_frame.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        #for c in cnts:
        #    (x, y, w, h) = cv2.boundingRect(c)
        #    cv2.rectangle(self.new_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cnts, _ = cv2.findContours(self.getFilteredImage(image), \
                                   cv2.RETR_EXTERNAL,            \
                                   cv2.CHAIN_APPROX_SIMPLE)
        for c in cnts:
            a = cv2.contourArea(c)
            if a > 0 and a < 1200:
                (x, y, w, h) = cv2.boundingRect(c)
                #print '(%d, %d), %d' % (x+w/2, y+h/2, a)

                """
                ** deleted lines **
                ret = demo_word_ocr_cv(image, x+w/2, y+h/2, 70, 20)
                if ret:
                    time.sleep(1.5)
                """
                # @TODO Calculate appropriate width and height.
                # For more details, see func() at main.py
                self.func(image, x+w/2, y+h/2, 70, 20) # @@ADDED

                cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)




if __name__ == "__main__":
    # lt = LaserTracker()
    pass