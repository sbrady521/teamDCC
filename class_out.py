import numpy, cv2, dynamic_cc

ORANGE_BGR = (51, 153, 255)
GREEN_BGR = (0, 255, 0)
UNCLASS_BGR = (255, 0, 255)
WHITE_BGR = (255, 255, 255)

# Map a color to bgr
def c2bgr(color):
    if color == dynamic_cc.ORANGE: return ORANGE_BGR
    if color == dynamic_cc.GREEN: return GREEN_BGR
    if color == dynamic_cc.WHITE: return WHITE_BGR
    return UNCLASS_BGR

# Stores representation of calibration
# With classified pixels highlighted
class ClassifiedImage:
    
    # Initialise with NumPy image representation
    def __init__(self, height, width, original_file_name):
        self.img = numpy.zeros((height, width, 3), numpy.uint8)
        self.img[:,:] = UNCLASS_BGR
        self.path = original_file_name.rstrip('.jpg') + '.classified.png'
        pass

    
    # Set pixel in image to a classified colour
    def classify(self, x, y, color):
        self.img[x, y] = c2bgr(color)
    
    # Write out new image containing highlighted classifications
    def write(self):
        cv2.imwrite(self.path, self.img)


if __name__ == '__main__':
    # test
    ClassifiedImage(200, 200, 'test.jpg').write()

