import sys
import numpy
import cv2

data_file_name = sys.argv[1]
out_file_name = sys.argv[2]

new_img = numpy.zeros((480, 640, 3), numpy.uint8)

rgbs = list()
with open(data_file_name, 'r') as data_file:
    for line in data_file:
        rgb = line.split(" ")
        rgb[0] = int(rgb[0])
        rgb[1] = int(rgb[1])
        rgb[2] = int(rgb[2])
        rgb.reverse()
        rgbs.append(rgb)

print len(rgbs)
cnt = 0
for xval in xrange(0, 640):
    for yval in xrange(0, 480):
        try:
            new_img[yval, xval] = rgbs[cnt]
        except IndexError:
            pass
        cnt += 1

cv2.imwrite(out_file_name, new_img)