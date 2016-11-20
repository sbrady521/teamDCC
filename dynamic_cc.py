import numpy
import cv2
import itertools

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
        self.classification = None

    def getClassification(self):
        #returns the color classification of the voxel (or unclassified)
        return self.classification

    def setClassification(self, classification):
        self.classification = classification

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

    #returns a list of voxels in a specified class TODO
    def getVoxelsInClass(self, colorClass):
        return

    #returns the voxel opposite to voxel in reference to reference TODO
    def oppositeVoxel(self, reference, voxel):
        return

    #returns whether a voxel is internal - ie 6 neighbouring voxels of the same class TODO
    def isInternal(self, voxel):
        return False

    def getRemovableVoxels(self, colorClass): #Gary TODO
        voxels = getVoxelsInClass(colorClass)
        removableVoxels = list()
        for voxel in voxels:
            rule1 = True
            internalCnt = 0 #count of internal neighbours

            #check rule 1
            neighbours = self.getNeighbours(voxel)
            internalNeighbours = list() #List of Internal Neighbours, for rule 2
                                        #filled during rule 1 checking for performance
            for neighbour in neighbours:
                if neighbour.getClassification() == voxel.getClassification() \
                 and neighbour.oppositeVoxel(voxel, neighbour) in neighbours:
                    rule1 = False
                if self.isInternal(neighbour):
                    internalCnt += 1
                    internalNeighbours.append(neighbour)
            if rule1:
                removableVoxels.append(voxel)
                continue

            #To pass rule 2 a voxel must have at least 2 internal neighbours
            if internalCnt < 2:
                continue

            #check rule 2
            validNeighbours = list()
            for neighbour in internalNeighbours:
                if self.oppositeVoxel(voxel, neighbour).getClassification() not in validNeighbours:
                    validNeighbours.append(neighbour)

            #Find neighbours which share a common neighbour with another neighbour
            #which isn't the original voxel
            for voxel1, voxel2 in itertools.permutations(validNeighbours, 2):
                voxel1Neighbours = self.getNeighbours(voxel1)
                voxel2Neighbours = self.getNeighbours(voxel2)
                for neighbour in voxel1Neighbours:
                    if neighbour != voxel and neighbour in voxel2Neighbours:
                        validNeighbours.append(neighbour)

            if len(validNeighbours) >= 2:
                removableVoxels.append(voxel)
        return removableVoxels

    #Perform shedding on the given class
    def performShedding(self, colorClass): #GARY TODO
        removableVoxels = getRemovableVoxels(colorClass)

        averageVotes = 0
        for voxel in removableVoxels:
            averageVotes += voxel.getVotes()
        averageVotes = averageVotes / len(removableVoxels)

        #check every removable voxel to see if their votes are too low
        #unclassify them if so
        for voxel in removableVoxels:
            if voxel.getVotes() < averageVotes:
                voxel.setClassification(None)
                voxel.setVotes(0)
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