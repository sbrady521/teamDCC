import numpy
import matplotlib.pyplot as plt
import cv2
import os
import sys

#img = cv2.imread("top-frame/test.png")
#img = cv2.cvtColor(img, cv2.COLOR_BGR2YUV)

"""
color = ('r','g','b')
for i,col in enumerate(color):
    histr = cv2.calcHist([img],[i],None,[256],[0,256])
    plt.plot(histr,color = col)
    plt.xlim([0,256])
plt.show()
"""
# Uses g_list sampled from image to classify the whole image
def classify(filename):

    # Get the minimum and maximum ranges of the histogram
    max_u = 128 
    max_v = 132

    img = cv2.imread(filename, cv2.IMREAD_COLOR)

    img = cv2.cvtColor(img, cv2.COLOR_BGR2YUV)
    
    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    # Initialize a new blank (black) image
    new_img = numpy.zeros((img_h, img_w, 3), numpy.uint8)

    for yval in xrange(0, img_h):
        for xval in xrange(0, img_w):
            # iterate through every pixel and get the sum of the b,g,r values


            # if the pixels green chromaticity values lie within the range
            # classify it by making it blue
            #print img[yval][xval][1]
            #print img[yval][xval][2] 
            ych = img[yval][xval][0]
            if ych >= 50 and ych <= 75:
                ych = 0
            else: 
                ych = 0 # TODO: Something like this maybe? ych/10
            #print ych
            if img[yval][xval][1] < (max_u - ych) and img[yval][xval][2] < (max_v - ych):
                new_img[yval, xval] = [255, 112, 132]

    # write the classified image
    new_filename = filename + "_rgchroma.png"

    cv2.imwrite(new_filename, new_img)

def main(args):
    if len(args) != 2:
        print "Usage: ./im_2_rgchroma [image directory]"
        sys.exit(1)

    #dir_list_bot = os.listdir(args[1] + str("/bottom_camera"))
    dir_list_top = os.listdir(args[1] + str("/top_camera"))
    #dir_list_bot.sort()
    dir_list_top.sort()

    # Iterate through files in the given dir
    ranges = []
    for i in xrange(0, len(dir_list_top)):
        #file_bot = dir_list_bot[i]
        file_top = dir_list_top[i]
        if file_top.endswith('_rgchroma.png') or file_top.endswith('g_values.png'):
            # skip converted files and graphs
            continue
        #filename = args[1] + '/bottom_camera/' + file_bot
        filename_top = args[1] + "/top_camera/" + file_top
        #saturation_show(filename_top)
        classify(filename_top)


if __name__ == '__main__':
    main(sys.argv)
