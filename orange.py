#/usr/bin/python3

import cv2

def convertToYUV(BGR):
    B = BGR[0]
    G = BGR[1]
    R = BGR[2]

    Y  = int(R *  0.29900 + G *  0.58700 + B *  0.11400)
    U = int(R * -0.16874 + G * -0.33126 + B *  0.50000 + 128)
    V = int(R *  0.50000 + G * -0.41869 + B * -0.08131 + 128)
    return (Y, U, V)

def to_yuv(bgr):
    b, g, r = bgr
    y = int(0.299 * r + 0.587 * g + 0.114 * b)
    u = int(-0.1687 * r - 0.3313 * g + 0.5 * b + 128)
    v = int(0.5 * r - 0.41869 * g - 0.08131 * b + 128)
    return (y, u, v)

img = cv2.imread('SAMPLE.jpg')
px_vals = list()
y_s = []
u_s = []
v_s = []

height, width = img.shape[:2]
# print height, width
for xval in xrange(width - 1):
    for yval in xrange(height):
        bgr = img[xval, yval]
        if not (bgr[0] > 220 and bgr[1] > 220 and bgr[2] > 220):
            # print ', '.join([str(x) for x in bgr])
            px  =   to_yuv(bgr)
            px_vals.append(px)

px_vals.sort(key = sum) 
# max
print px_vals[0]
# min
print px_vals[-100]