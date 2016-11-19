import numpy
import cv2

class Voxel:
    #Voxel class
    #A Voxel is a specific YUV value

    def __init__(self, bgr):
        #Initialisation
        assert isinstance(bgr, object)
        self.bgr = bgr
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
        return

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

red_voxel = Voxel(image[0,50])

print red_voxel.getBGR()