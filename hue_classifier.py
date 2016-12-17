import numpy, cv2
import matplotlib.pyplot as pyplot
import sys
import os
import subprocess
import random
#from rpy2.robjects import r
# ATTN:
# Currently uses R to plot the range of g chroma values
# because R has more powerful plotting facilities than python
# but if you don't have R installed you can comment out the last line
# in main() - R_plot_g_values call
# and uncomment the plot_g_values call above it
# This will plot a shittier graph with matplotlib.pyplot
# You also need to install rpy2 (pip install rpy2 should be sufficient)

# Samples the bottom 20% of the image and returns
# a list of g chromacity values from that sample
def sample(filedir):
    img = cv2.imread(filedir, cv2.IMREAD_COLOR)

    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    # Convert to HSV
    cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    # Initialize an array to store g values for analysis purposes
    g_values = list()

    for yval in xrange(int(img_h*0.625), img_h):
        for xval in xrange(0, img_w):

            # add g values to g value list if the pixel is mostly green
            h = img[yval, xval, 0]
            g_values.append(h)

    return g_values

#Scan the top images colors with 3 strips vertically and horizontally
def sample_top(filedir, g_min, g_max):
    img = cv2.imread(filedir, cv2.IMREAD_COLOR)

    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    top_values = []

    #split the images into 3 pieces both horizontally and vertically
    x_quarts = [img_w/4, img_w/2, img_w*3/4]
    y_quarts = [img_h/4, img_h/2, img_h*3/4]

    #search for green pixels in 3 horizontal strips
    for quart_x in xrange(0, len(x_quarts)):
        for x in xrange(0, img_w):
            rgb_sum = float(sum(img[y_quarts[quart_x], x]))
            if rgb_sum == 0:
                continue
            g_chroma = (float(img[y_quarts[quart_x], x, 1])/rgb_sum) * 255.0

            #add the green pixel if it is within 3 chroma values from the current maximum or minimum
            if (g_max < g_chroma < g_max + 3) or (g_min - 3 < g_chroma < g_min):
                top_values.append(g_chroma)
                #print "adding " + str(g_chroma) + " from horizontal top"

    #Do the same vertically
    for quart_y in xrange(0, len(y_quarts)):
        for y in xrange(img_h - 1, -1, -1):
            rgb_sum = float(sum(img[y,x_quarts[quart_y]]))
            if rgb_sum == 0:
                continue
            g_chroma = (float(img[y, x_quarts[quart_y], 1])/rgb_sum) * 255.0

            if (g_max < g_chroma < g_max + 3) or (g_min - 3 < g_chroma < g_min):
                top_values.append(g_chroma)
                #print "adding " + str(g_chroma) + " from top vertical scan"
    return top_values


def diagonal_sample(filename):
    img = cv2.imread(filename, cv2.IMREAD_COLOR)

    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    # Initialize an array to store g values for analysis purposes
    g_values = list()

    ycnt = int(img_h*0.2)
    xcnt = 0
    ratio = int(img_w/img_h)

    while ycnt < img_h:
        xstart = ycnt*ratio
        xend = xstart + 20 if (xstart + 20) < img_w else img_w
        xcnt = xstart
        while xcnt < xend:
            # iterate through every pixel and get the sum of the b,g,r values
            rgb_sum = float(sum(img[ycnt,xcnt]))
            if rgb_sum == 0:
                continue
            # calculate the new chromatic value for each channel
            # 255 is a magic number which scales the image value so it can be displayed
            # in an rgb format
            # TODO remove magic number?
            # b_chroma = (float(img[yval, xval, 0])/rgb_sum) * 255.0
            g_chroma = (float(img[ycnt, xcnt, 1])/rgb_sum) * 255.0
            # r_chroma = (float(img[yval, xval, 2])/rgb_sum) * 255.0

            g_values.append(g_chroma)
            xcnt += 1
        ycnt += 1

    return g_values

def three_strips_sample(filename):
    img = cv2.imread(filename, cv2.IMREAD_COLOR)

    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    # Initialize an array to store g values for analysis purposes
    g_values = list()

    for yval in xrange(int(img_h*0.2), int(img_h*0.3)):
        for xval in xrange(0, img_w):
            # iterate through every pixel and get the sum of the b,g,r values
            rgb_sum = float(sum(img[yval,xval]))
            if rgb_sum == 0:
                continue
            g_chroma = (float(img[yval, xval, 1])/rgb_sum) * 255.0

            g_values.append(g_chroma)

    for yval in xrange(int(img_h*0.5), int(img_h*0.6)):
        for xval in xrange(0, img_w):
            # iterate through every pixel and get the sum of the b,g,r values
            rgb_sum = float(sum(img[yval,xval]))
            if rgb_sum == 0:
                continue
            g_chroma = (float(img[yval, xval, 1])/rgb_sum) * 255.0

            g_values.append(g_chroma)

    for yval in xrange(int(img_h*0.8), int(img_h*0.9)):
        for xval in xrange(0, img_w):
            # iterate through every pixel and get the sum of the b,g,r values
            rgb_sum = float(sum(img[yval,xval]))
            if rgb_sum == 0:
                continue
            g_chroma = (float(img[yval, xval, 1])/rgb_sum) * 255.0

            g_values.append(g_chroma)

    return g_values

def random_sample(filename):
    img = cv2.imread(filename, cv2.IMREAD_COLOR)

    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    # Initialize an array to store g values for analysis purposes
    g_values = list()

    cnt = 0
    while cnt < 20000:
        xval = random.randint(0, img_w - 1)
        yval = random.randint(0, img_h - 1)

        rgb_sum = float(sum(img[yval,xval]))
        if rgb_sum == 0:
            continue
        g_chroma = (float(img[yval, xval, 1])/rgb_sum) * 255.0

        g_values.append(g_chroma)

        cnt += 1

    return g_values

# plots a list of g_values using matplotlib in python
def plot_g_values(g_list, filename):
    # Clear plot
    pyplot.clf()

    # plot g values into a 1d scatter
    #pyplot.plot(g_list)

    hist = numpy.histogram(g_list, bins='sturges', density=True)
    print hist
    # save plot
    plot_filename = filename + '_g_values.png'
    pyplot.savefig(plot_filename)

# plots a list of g_values using R
def R_plot_g_values(g_list, filename):
    # write the list to a temporary file
    new_file = open("g_list", 'w')
    for val in g_list:
        new_file.write(str(val) + "\n")

    new_file.close()

    # plot graph using R
    filename = filename + '_g_values.png'
    print filename
    #r('g_vals <- read.table("g_list")')
    #r('png(filename = "' + filename + '", width=1000, height = 255)')
    #r('smoothScatter(g_vals[[1]], y = NULL, ylab = "g_chroma values", xlab = "pixel number")')
    #r('dev.off()')

    #call the R script
    #os.system("./plot_g_values.R " + "g_list" + " " + filename)
    subprocess.call("Rscript " + " plot_g_values.R " + "g_list" + " " + filename)

# Uses g_list sampled from image to classify the whole image
def classify(g_list, filename):
    # Convert the g_list to a histogram
    hist = numpy.histogram(g_list, bins='doane', density=True)

    # Get the minimum and maximum ranges of the histogram
    min_g, max_g = get_g_range(hist)

    print min_g, max_g

    img = cv2.imread(filename, cv2.IMREAD_COLOR)

    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    # Convert to HSV
    cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    # Initialize a new blank (black) image
    new_img = numpy.zeros((img_h, img_w, 3), numpy.uint8)

    for yval in xrange(0, img_h):
        for xval in xrange(0, img_w):
            # iterate through every pixel and get the hue
            hue = img[yval, xval, 0]
            # if the pixels green chromaticity values lie within the range
            # classify it by making it blue
            if min_g <= hue <= max_g:
                new_img[yval, xval] = [255, 112, 132]

    # write the classified image
    new_filename = filename + "_rgchroma.png"

    cv2.imwrite(new_filename, new_img)

# Gets the minimum and maximum bin values for a g_list
def get_g_range(g_list):
    # If the bin density is above a threshold it is a valid bin
    threshold = 0.035

    min_g = None
    max_g = None
    max_index = None

    cnt = 0
    while cnt < len(g_list[0]):
        if g_list[0][cnt] > threshold:
            if min_g is None:
                min_g = g_list[1][cnt]
            elif max_g is not None and g_list[0][cnt] > g_list[0][max_index]:
                min_g = g_list[1][cnt]
                max_g = None
        else:
            if min_g is not None and max_g is None:
                max_g = g_list[1][cnt]
                max_index = cnt

        cnt += 1

    if max_g is None:
        max_g = 120
    if min_g is None:
        min_g = 100
    # Feather the mininum / maximum sligthly
    return min_g - 7, max_g + 100

def flattedArray(oldArray):
    newArray = []
    for i in xrange(0, len(oldArray)):
        for x in xrange(0, len(oldArray[i])):
            newArray.append(oldArray[i][x])
    return newArray

def main(args):
    if len(args) != 2:
        print "Usage: ./im_2_rgchroma [image directory]"
        sys.exit(1)

    dir_list_bot = os.listdir(args[1] + str("/bottom_camera"))
    dir_list_top = os.listdir(args[1] + str("/top_camera"))
    dir_list_bot.sort()
    dir_list_top.sort()

    # Iterate through files in the given dir
    ranges = []
    for i in xrange(0, len(dir_list_bot)):
        file_bot = dir_list_bot[i]
        file_top = dir_list_top[i]
        if file_top.endswith('_rgchroma.png') or file_top.endswith('g_values.png'):
            # skip converted files and graphs
            continue
        filename = args[1] + '/bottom_camera/' + file_bot
        filename_top = args[1] + "/top_camera/" + file_top
        g_values = sample(filename)
        ranges.append(g_values)
        if len(ranges) > 5:
            ranges.pop(0)
        #top_values = []
        #top_values = sample_top(filename_top, min(g_values), max(g_values))
        # g_values = flattedArray(ranges)
        #if len(top_values) > 0:
        #    g_values.extend(top_values)
        #plot_g_values(g_values, filename)
        R_plot_g_values(g_values, filename)
        classify(g_values, filename_top)


if __name__ == '__main__':
    main(sys.argv)
