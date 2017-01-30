import sys
import numpy
import cv2
import os

class Point:

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def getX(self):
        return self.x

    def getY(self):
        return self.y

data_file_name = sys.argv[1]
out_file_name = sys.argv[2]

points = list()
with open(data_file_name, 'r') as data_file:
    for line in data_file:
        coords = line.split(" ")
        points.append(Point(int(coords[0]), int(coords[1])))

new_img = numpy.zeros((960,1280,3), numpy.uint8)

for point in points:
    new_img[point.y, point.x] = [255, 112, 132]

cv2.imwrite(out_file_name, new_img)