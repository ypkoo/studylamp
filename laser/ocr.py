import cv, cv2
import pytesseract
from PIL import Image, ImageEnhance
import socket, time, enchant

UDP_IP = "127.0.0.1"
UDP_PORT = 12345
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
    # img_path = str(time.time())+".png"
    img_path = "image/"+str(time.time())+".png"
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
