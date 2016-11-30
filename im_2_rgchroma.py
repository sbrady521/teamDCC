import numpy, cv2
import matplotlib.pyplot as pyplot
import sys
import os
import subprocess
#from rpy2.robjects import r
# ATTN:
# Currently uses R to plot the range of g chroma values
# because R has more powerful plotting facilities than python
# but if you don't have R installed you can comment out the last line
# in main() - R_plot_g_values call
# and uncomment the plot_g_values call above it
# This will plot a shittier graph with matplotlib.pyplot
# You also need to install rpy2 (pip install rpy2 should be sufficient)

# Converts an image file to rg chroma and writes it
# also returns a list of g values
def convertToRGChroma(filedir):
    img = cv2.imread(filedir, cv2.IMREAD_COLOR)

    if img is None:
        print "Failed to read or load the image at" + str(filedir)
        return

    # Get Image dimensions
    img_h, img_w = img.shape[:2]

    # Initialize a new blank (black) image
    new_img = numpy.zeros((img_h, img_w, 3), numpy.uint8)

    # Initialize an array to store g values for analysis purposes
    g_values = list()

    print img_w
    print img_h

    for yval in xrange(0, img_h):
        for xval in xrange(0, img_w):
            # iterate through every pixel and get the sum of the b,g,r values
            rgb_sum = float(sum(img[yval,xval]))
            if rgb_sum == 0:
                continue
            # calculate the new chromatic value for each channel
            # 255 is a magic number which scales the image value so it can be displayed
            # in an rgb format
            # TODO remove magic number?
            b_chroma = (float(img[yval, xval, 0])/rgb_sum) * 255.0
            g_chroma = (float(img[yval, xval, 1])/rgb_sum) * 255.0
            r_chroma = (float(img[yval, xval, 2])/rgb_sum) * 255.0
            # save chromatic values to the new image
            new_img[yval, xval] = [b_chroma, g_chroma, r_chroma]

            # add g values to g value list if the pixel is mostly green
            if (g_chroma/255.0 > 0.2):
                g_values.append(g_chroma)
            else:
                pass
                #print "skipped adding pixel " + str(xval) + " " + str(yval)


    new_filename = filedir + "_rgchroma.png"

    cv2.imwrite(new_filename, new_img)

    return g_values

# plots a list of g_values using matplotlib in python
def plot_g_values(g_list, filename):
    # Clear plot
    pyplot.clf()

    # plot g values into a 1d scatter
    pyplot.plot(g_list)

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
    #r('g_vals <- read.table("g_list")')
    #r('png(filename = "' + filename + '", width=1000, height = 255)')
    #r('smoothScatter(g_vals[[1]], y = NULL, ylab = "g_chroma values", xlab = "pixel number")')
    #r('dev.off()')

    #call the R script
    #os.system("./plot_g_values.R " + "g_list" + " " + filename)
    subprocess.call("Rscript " + " plot_g_values.R " + "g_list" + " " + filename)

def main(args):
    if len(args) != 2:
        print "Usage: ./im_2_rgchroma [image directory]"
        sys.exit(1)

    dir_list = os.listdir(args[1])
    dir_list.sort()

    # Iterate through files in the given dir
    for file in dir_list:
        if file.endswith('_rgchroma.png') or file.endswith('g_values.png'):
            # skip converted files and graphs
            continue
        filename = args[1] + '/' + file
        g_values = convertToRGChroma(filename)
        #plot_g_values(g_values, filename)
        R_plot_g_values(g_values, filename)


if __name__ == '__main__':
    main(sys.argv)