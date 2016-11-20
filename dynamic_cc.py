import numpy
import cv2

def convertToYUV(BGR):
    B = BGR[0]
    G = BGR[1]
    R = BGR[2]

    Y  = int(R *  0.29900 + G *  0.58700 + B *  0.11400)
    U = int(R * -0.16874 + G * -0.33126 + B *  0.50000 + 128)
    V = int(R *  0.50000 + G * -0.41869 + B * -0.08131 + 128)
    return [Y, U, V]

class Voxel:
    #Voxel class
    #A Voxel is a specific YUV value

    def __init__(self, pixel_vals):
        #Initialisation
        assert isinstance(pixel_vals, object)
        self.yuv = convertToYUV(pixel_vals)
        self.votes = 0

    def getClassification(self, class_list):
        #returns the color classification of the voxel (or unclassified)
        return None

    def getBGR(self):
        return self.bgr

    def getYUV(self):
        return self.yuv

    def setVotes(self, vote):
        self.votes = vote

    def getVotes(self):
        return self.votes



class Classification:
    #Colour Classification class
    #Keeps track of which colours are in the classification
    #and the surface area and volume of the classification

    def __init__(self):
        #Initialisation
        self.Volume = 0
        self.SurfaceArea = 0

    def getSurfaceArea(self):
        #Gets the surface area of the classification
        return self.SurfaceArea

    def getVolume(self):
        #Gets the volume of the classification
        return self.Volume


class LookUpTable:
    def __init__(self):
        self.LUT = initialiseLUT()
        self.greenClass = Classification()
        self.whiteClass = Classification()
        self.orangeClass = Classification()

    #Perform shedding on the given class
    def performShedding(self, colorClass): #GARY TODO
        
        return

    def getNeighbours(self, voxel): #GARY TODO
        #returns a list of the neighbouring voxels of the voxel
        #two voxels are considered neighbours if they share a face
        yuv_coords = voxel.getYUV() #get the coordinates of the specified voxel
        y = yuv_coords[0]
        u = yuv_coords[1]
        v = yuv_coords[2]
        neighbours = list() #of voxels
        #iterate through neighbours and append them to the list
        for i in xrange(y-1,y+1):
            for j in xrange(u-1,u+1):
                for k in xrange(v-1,v+1):
                    if i == y and j == u and k == v:
                        continue
                    neighbours.append(self.LUT[i][j][k])
        return neighbours


#This function needs to initialise a 3d array of voxels
#The voxels in the coordinate range specified by the parameters should be
#classified orange
def initialiseLUT(orangeYmax, orangeYmin, orangUmax, orangeUmin, orangeVmax, orangeVmin): #SEAN TODO
    return

def updateLUT(image, colorClass): #SEAN TODO
    return

def tests():
    return

def main():
    tests()


main()