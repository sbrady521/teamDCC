import numpy
import cv2
import itertools
import os
#lul
UNCLASS = 0
WHITE = 1
GREEN = 2
ORANGE = 3

MAXVOTES=100

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
        self.maxvotes = 100

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
        self.maxVol = 3300
        self.maxSA = 7000
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
    def __init__(self, orangeRanges):
        self.greenClass = Classification()
        self.whiteClass = Classification()
        self.orangeClass = Classification()
        self.LUT = initialiseLUT(self, orangeRanges)

    #returns a list of voxels in a specified class
    def getVoxelsInClass(self, colorClass):
        if colorClass == WHITE:
            returnVal = self.whiteClass.getClassVoxels()
        elif colorClass == GREEN:
            returnVal = self.greenClass.getClassVoxels()
        elif colorClass == ORANGE:
            returnVal = self.orangeClass.getClassVoxels()

        return returnVal

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
        neighbours = self.getNeighbours(voxel.getYUV())
        classification = voxel.getClassification()
        for neighbour in neighbours:
            if not neighbour:
                return False
            if neighbour.getClassification() != classification:
                return False
        return True

    def getRemovableVoxels(self, colorClass): #Gary TODO
        voxels = self.getVoxelsInClass(colorClass)
        removableVoxels = list()
        for voxel in voxels:
            rule1 = True
            internalCnt = 0 #count of internal neighbours

            #check rule 1
            neighbours = self.getNeighbours(voxel.getYUV())
            internalNeighbours = list() #List of Internal Neighbours, for rule 2
                                        #filled during rule 1 checking for performance
            for neighbour in neighbours:
                if not neighbour:
                    continue
                if neighbour.getClassification() == voxel.getClassification() \
                 and self.oppositeVoxel(voxel, neighbour) in neighbours:
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
                voxel1Neighbours = self.getNeighbours(voxel1.getYUV())
                voxel2Neighbours = self.getNeighbours(voxel2.getYUV())
                for neighbour in voxel1Neighbours:
                    if neighbour != voxel and neighbour in voxel2Neighbours:
                        validNeighbours.append(neighbour)

            if len(validNeighbours) >= 2:
                removableVoxels.append(voxel)
        return removableVoxels

    #Perform shedding on the given class
    def performShedding(self, colorClass): #GARY TODO
        removableVoxels = self.getRemovableVoxels(colorClass)

        averageVotes = 0
        for voxel in removableVoxels:
            averageVotes += voxel.getVotes()
        averageVotes = averageVotes / len(removableVoxels)

        #check every removable voxel to see if their votes are too low
        #unclassify them if so
        for voxel in removableVoxels:
            if voxel.getVotes() < averageVotes:
                #print "removing voxel YUV:"
                #print voxel.yuv
                yuv = voxel.getYUV()
                self.LUT[yuv[0],yuv[1],yuv[2]] = None
                if colorClass == ORANGE:
                    self.orangeClass.removeVoxel(voxel)
                elif colorClass == WHITE:
                    self.whiteClass.removeVoxel(voxel)
                elif colorClass == GREEN:
                    self.greenClass.removeVoxel(voxel)
        return

    def getNeighbours(self, yuv_coords): #GARY TODO
        #returns a list of the neighbouring voxels of the voxel
        #two voxels are considered neighbours if they share a face
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

    def updateLUT(self, image, colorClass):
        #Get image height and width
        height, width = image.shape[:2]

        #Decrement all votes in the specified color class
        decrementVotes(self, colorClass)

        #Loop through every pixel in the image
        for xval in xrange(0,width):
            for yval in xrange(0, height):
                #Analyse pixels yuv color and check LUT
                bgr = image[xval,yval]
                yuv = convertToYUV(bgr)
                currentVox = self.LUT[yuv[0]][yuv[1]][yuv[2]]

                #Check for unclassified pixels similar to observed color class
                if currentVox != None:
                    if currentVox.getClassification() == ORANGE:
                        print str(xval) + " " + str(yval)
                if not currentVox and isNeighbour(self, yuv, colorClass):
                    print
                    print "(adding voxel)"
                    print "YUV: " + str(yuv[0]) + " " + str(yuv[1]) + " " + str(yuv[2])
                    print "pixel: " + str(xval) + " " + str(yval)
                    print

                    self.LUT[yuv[0]][yuv[1]][yuv[2]] = Voxel(yuv)
                    currentVox = self.LUT[yuv[0]][yuv[1]][yuv[2]]
                    currentVox.setClassification(colorClass)
                    currentVox.setVotes(0)

                #Increment votes of seen class
                elif currentVox and currentVox.classification == colorClass:
                    currentVox.incrementVote()

        #Check class volume and surface area
        if colorClass == ORANGE:
            classObject = self.orangeClass
        elif colorClass == WHITE:
            classObject = self.whiteClass
        else:
            classObject = self.greenClass
        curVol = classObject.getVolume
        curSA = classObject.getSurfaceArea

        if curVol > classObject.maxVol or curSA > classObject.maxSA:
            self.performShedding(colorClass)


#This function needs to initialise a 3d array of voxels
#The voxels in the coordinate range specified by the parameters should be
#classified orange
def initialiseLUT(mainLut, orangeArr):
    LUT = numpy.empty([256,256,256], object)
    orangeYmax = orangeArr[0]
    orangeYmin = orangeArr[1]
    orangeUmax = orangeArr[2]
    orangeUmin = orangeArr[3]
    orangeVmax = orangeArr[4]
    orangeVmin = orangeArr[5]

    #Create 3d array of voxels for LUT
    for y in xrange(orangeYmin, orangeYmax):
        for u in xrange(orangeUmin, orangeUmax):
            for v in xrange(orangeVmin, orangeVmax):
                tempVox = Voxel([y,u,v]);

                #Check if voxel lies in parameter space
                if (orangeYmin < y < orangeYmax) and (orangeUmin < u < orangeUmax) and (orangeVmin < v < orangeVmax):
                    tempVox.setVotes(MAXVOTES)
                    tempVox.setClassification(ORANGE)
                    mainLut.orangeClass.addVoxel(tempVox)
                LUT[y,u,v] = tempVox

    return LUT

#Decrement all the votes in a color class
def decrementVotes(mainLUT, colorClass):
    voxelArr = mainLUT.getVoxelsInClass(colorClass)
    for voxel in voxelArr:
        voxel.decrementVote()


#Given a voxel and a class, check if any neighbouring voxels have the given color class
def isNeighbour(mainLUT, yuv, colorClass):
    neighbours = mainLUT.getNeighbours(yuv)
    for currNeighbour in neighbours:
        if currNeighbour != None:
            if currNeighbour.getClassification() == colorClass:
                print "Voxel has classified neighbour"
                yuv = currNeighbour.getYUV()
                print "with YUV: " + str(yuv[0]) + " " + str(yuv[1]) + " " + str(yuv[2])
                return True
    return False

def tests():
    #153 93 197
    #87 99 187
    return

def main():
    #range values [Ymax, Ymin, Umax, Umin, Vmax, Vmin]
    orangeArray = [227, 132, 81, 74, 207, 147]
    mainLUT = LookUpTable(orangeArray)

    #Fill images with images in test folder
    images = []
    dirList = os.listdir('PhotoDataSmall')
    dirList.sort()
    for photo in dirList:
        filename = "PhotoDataSmall/"+str(photo)
        images.append(cv2.imread(filename))

    i = 0
    for image in images:
        height, width = image.shape[:2]
        print
        print
        print "analysing image " + str(i)
        print "%d x %d" % (width, height)
        print
        print
        mainLUT.updateLUT(image, ORANGE)
        i+=1


    #tests()


main()
