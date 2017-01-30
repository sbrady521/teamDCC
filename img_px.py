# Usage: python test1.py [filename] [x-coord] [y-coord]
# Prints out RGB and Y'UV value of selected pixel in image

import cv2
import sys
img = cv2.imread(sys.argv[1])

def to_yuv(bgr):
    b, g, r = bgr
    y = int(0.299 * r + 0.587 * g + 0.114 * b)
    u = int(-0.1687 * r - 0.3313 * g + 0.5 * b + 128)
    v = int(0.5 * r - 0.41869 * g - 0.08131 * b + 128)
    return (y, u, v)

b, g, r = img[int(sys.argv[2]), int(sys.argv[3])]
print r, g, b
print to_yuv((b, g, r))
