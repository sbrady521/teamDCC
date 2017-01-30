import cv2

image = cv2.imread("red_box.jpg")

red = image[0,50,2]
white = image[100,100,2]
height, width = image.shape[:2]
red_cnt = 0
white_cnt = 0

for xval in xrange(0,width):
    for yval in xrange(0, height):
        if image[xval,yval,2] == red:
            red_cnt += 1
        elif image[xval, yval, 2] == white:
            white_cnt += 1

print red_cnt
print white_cnt