__author__ = 'young'

from PIL import Image
from numpy import *
from scipy.ndimage import filters
from scipy.misc import imsave
import cv2
import cv2.cv as cv
import numpy as np

def medianCanny(img, thresh1, thresh2):
    median = np.median(img)
    img = cv2.Canny(img, int(thresh1 * median), int(thresh2 * median))
    return img

# read image
img_orig = cv2.imread("raw_images/desk.jpg")

# convert to grayscale
img = cv2.cvtColor(img_orig,cv2.cv.CV_RGB2GRAY)

# apply gaussian blur
img = cv2.GaussianBlur(img, (5,5), 0)

# thresholding
flag, img = cv2.threshold(img,0,255,cv2.THRESH_OTSU)

# find edges from the thresholding img
edges = medianCanny(img, 0.2, 0.3)

contours,hierarchy = cv2.findContours(edges, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
hierarchy = hierarchy[0]

for component in zip(contours, hierarchy):
    currentContour = component[0]
    currentHierarchy = component[1]
    x,y,w,h = cv2.boundingRect(currentContour)
    if currentHierarchy[2] < 0:
        # these are the innermost child components
        cv2.rectangle(img_orig,(x,y),(x+w,y+h),(0,0,255),3)
    elif currentHierarchy[3] < 0:
        # these are the outermost parent components
        cv2.rectangle(img_orig,(x,y),(x+w,y+h),(0,255,0),3)


# display
# cv2.imshow('img',img_orig)
# cv2.waitKey(0)
# cv2.destroyAllWindows()

cv2.imwrite("desk-rect.jpg", img_orig)