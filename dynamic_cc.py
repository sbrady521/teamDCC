import numpy
import cv2
import itertools

UNCLASS = 0
WHITE = 1
GREEN = 2
ORANGE = 3

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

        self.yuv = pixel_vals
        self.votes = 0
        self.classification = UNCLASS
        self.incVal = 1
        self.decVal = 1
        self.maxvotes = 10

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

    def incrementVote(self):
        self.votes += self.incVal

        #ensure votes don't exceed max
        if self.votes > 10:
            self.votes = 10

    def decrementVote(self):
        self.votes -= self.decVal

        #Ensure votes do not go in the negatives
        if self.votes < 0:
            self.votes = 0



class Classification:
    #Colour Classification class
    #Keeps track of which colours are in the classification
    #and the surface area and volume of the classification

    def __init__(self):
        #Initialisation
        self.Volume = 0
        self.SurfaceArea = 0
        self.classVoxels = []

    def getSurfaceArea(self):
        #Gets the surface area of the classification
        return self.SurfaceArea

    def getVolume(self):
        #Gets the volume of the classification
        return self.Volume

    def addVoxel(self, voxel):
        self.classVoxels.append(voxel)

    def removeVoxel(self, voxel):
        i = 0
        for currVoxel in self.classVoxels:
            if(currVoxel.getYUV() == voxel.getYUV()):
                self.classVoxels.pop(i)
                break
            i += 1

    def getClassVoxels(self):
        return self.classVoxels



class LookUpTable:
    def __init__(self):
        self.LUT = initialiseLUT()
        self.greenClass = Classification()
        self.whiteClass = Classification()
        self.orangeClass = Classification()
        self.unclassifiedClass = Classification()

    #returns a list of voxels in a specified class
    def getVoxelsInClass(self, colorClass):
        if colorClass == WHITE:
            returnVal = self.whiteClass.getVoxelsInClass()
        elif colorClass == GREEN:
            returnVal = self.greenClass.getVoxelsInClass()
        elif colorClass == ORANGE:
            returnVal = self.orangeClass.getVoxelsInClass()
        else:
            returnVal = self.unclassifiedClass.getVoxelsInClass()

        return returnVals

    #returns the voxel opposite to voxel in reference to reference TODO
    def oppositeVoxel(self, reference, voxel):
        voxelYUV = voxel.getYUV()
        referenceYUV = reference.getYUV()
        if voxelYUV[0] != referenceYUV[0]:
            if referenceYUV[0] == 255:
                return None
            return self.LUT[referenceYUV[0] - (voxelYUV[0] - referenceYUV[0])][voxelYUV[1]][voxelYUV[2]]
        elif voxelYUV[1] != referenceYUV[1]:
            if referenceYUV[1] == 255:
                return None
            return self.LUT[voxelYUV[0]][referenceYUV[1] - (voxelYUV[1] - referenceYUV[1])][voxelYUV[2]]
        else:
            if referenceYUV[2] == 255:
                return None
            return self.LUT[voxelYUV[0]][voxelYUV[1]][referenceYUV[2] - (voxelYUV[2] - referenceYUV[2])]
        return None

    #returns whether a voxel is internal - ie 6 neighbouring voxels of the same class TODO
    def isInternal(self, voxel):
        neighbours = self.getNeighbours(voxel)
        classification = voxel.getClassification()
        for neighbour in neighbours:
            if neighbour.getClassification() != classification:
                return False
        return True

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
                if self.oppositeVoxel(voxel, neighbour) not in validNeighbours:
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
                voxel.setClassification(UNCLASS)
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
def initialiseLUT(orangeYmax, orangeYmin, orangUmax, orangeUmin, orangeVmax, orangeVmin):
    mainLut = []

    #Create 3d array of voxels for LUT
    for y in xrange(257):
        tempLut1 = []
        for u in xrange(257):
            tempLut2 = []
            for v in xrange(257):
                tempVox = Voxel([y,u,v]);

                #Check if voxel lies in parameter space
                if (orangeYmin < y < orangeYmax) and (orangeUmin < u < orangeUmax) and (orangeVmin < v < orangeVmax):
                    tempVox.setVotes(MAXVOTES)
                    tempVox.setClassification(ORANGE)
                    mainLut.orangeClass.addVoxel(tempVox)

                #Add to the 1d array
                tempLut2.append(tempVox)

            #Add to the 2d array
            tempLut1.append(tempLut2)

        #Add to the 3d array
        mainLut.append(tempLut1)

def updateLUT(mainLUT, image, colorClass): #SEAN TODO
    #Get image height and width
    height, width = image.shape[:2]

    #Decrement all votes in the specified color class
    decrementVotes(mainLUT, colorClass)

    #Loop through every pixel in the image
    for xval in xrange(0,width):
        for yval in xrange(0, height):

            #Analyse pixels yuv color and check LUT
            bgr = image[xval,yval]
            yuv = convertToYUV(bgr)
            currentVox = mainLUT.LUT[yuv[0]][yuv[1]][yuv[2]]

            #Increment votes of seen class
            if currentVox.classification == colorClass:
                currentVox.incrementVote()

            #Check for unclassified pixels similar to observed color class
            elif currentVox.classification == UNCLASS and isNeighbour(mainLUT, currentVox, colorClass):
                currentVox.setClassification(colorClass)
                currentVox.setVotes(0)

    #Check class volume and surface area
    if colorClass == ORANGE:
        classObject = LookUpTable.orangeClass
    elif colorClass == WHITE:
        classObject = LookUpTable.whiteClass
    else:
        classObject = LookUpTable.greenClass
    curVol = classObject.getVolume
    curSA = classObject.getSurfaceArea

    if curVol > MAXCLASSVOL or curSA > MAXCLASSA:
        mainLUT.performShedding(colorClass)

    return mainLUT

#Decrement all the votes in a color class
def decrementVotes(mainLUT, colorClass):
    voxelArr = mainLUT.getVoxelsInClass(colorClass)
    for voxel in voxelArr:
        voxel.decrementVote()


#Given a voxel and a class, check if any neighbouring voxels have the given color class
def isNeighbour(mainLUT, vox, colorClass):
    neighbours = mainLUT.getNeighbours(vox)
    for currNeighbour in neighbours:
        if currNeighbour.getClassification == colorClass:
            return True
    return False

def tests():
    return

def main():
    tests()


main()
