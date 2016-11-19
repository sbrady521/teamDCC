import numpy
import cv2

class Voxel:
    #Voxel class
    #A Voxel is a specific YUV value

    def __init__(self, pixel_vals, yuv = True):
        #Initialisation
        assert isinstance(pixel_vals, object)
        if yuv:
            self.yuv = pixel_vals
            self.bgr = None
        else:
            self.bgr = pixel_vals
            self.yuv = None
        #We need to convert bgr to yuv

    def getClassification(self, class_list):
        #returns the color classification of the voxel (or unclassified)
        return None

    def getNeighbours(self):
        #returns a list of the neighbouring voxels of the voxel
        return

    def getBGR(self):
        return self.bgr

    def getYUV(self):
        return self.yuv

class Classification:
    #Colour Classification class
    #Keeps track of which colours are in the classification
    #and the surface area and volume of the classification

    def __init__(self):
        #Initialisation
        self.Volume = 0
        self.SurfaceArea = 0
        self.LUT = numpy.zeros((255,255,255))

    def getSurfaceArea(self):
        #Gets the surface area of the classification
        return 0

    def getVolume(self):
        #Gets the volume of the classification
        return 0

    def isInClassification(self, voxel):
        #Returns whether a voxel is part of this classification
        return False

    def addVoxel(self, voxel):
        #Adds a voxel to the classification
        #Will need to update Surface Area and Volume

        return

    def getLUT(self):
        return self.LUT

image = cv2.imread("red_box.jpg")

red_yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV)
red_val = red_yuv[0,50]
red_rgb = cv2.cvtColor(red_yuv, cv2.COLOR_YUV2RGB)
red_yuv2 = cv2.cvtColor(red_rgb, cv2.COLOR_RGB2YUV)
print red_yuv[0,50]
print red_rgb[0,50]
print red_yuv2[0,50]