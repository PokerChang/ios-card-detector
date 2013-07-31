__author__ = 'young'

from numpy import *
from scipy.ndimage import filters
from scipy.misc import imsave
import cv2
import cv2.cv as cv
import numpy as np
import os

NEXT,PREV,FIRST_CHILD,PARENT=0,1,2,3
MAX_CARD_AREA = 50000
MIN_CARD_AREA = 10000
ELEMENT = cv2.getStructuringElement(cv2.MORPH_CROSS,(3,3))
ELEMENT_ELLIPSE = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(3,3))

scale = 1
delta = 0
ddepth = cv2.CV_16S

def show(img):
    cv2.imshow("img", img)
    cv2.waitKey(0)

# thresholding grey scaled img
def preprocess(img):
    img = cv2.cvtColor(img, cv2.cv.CV_RGB2GRAY)
    img = cv2.GaussianBlur(img, (3, 3), 0)
    img = cv2.adaptiveThreshold(img, 255, 1, 1, 7, 2)
    img = cv2.morphologyEx(img, cv2.MORPH_CLOSE, ELEMENT_ELLIPSE, iterations=1)
    return img

# thresholding grey scaled img
def preprocess1(img):
    img = cv2.cvtColor(img, cv2.cv.CV_RGB2GRAY)
    img = cv2.GaussianBlur(img, (3, 3), 0)
    img = cv2.adaptiveThreshold(img, 255, 1, 1, 11, 2)
    img = cv2.dilate(img, ELEMENT)
    # Gradient-X
    grad_x = cv2.Sobel(img,ddepth,1,0,ksize = 3, scale = scale, delta = delta,borderType = cv2.BORDER_DEFAULT)
    #grad_x = cv2.Scharr(gray,ddepth,1,0)

    # Gradient-Y
    grad_y = cv2.Sobel(img,ddepth,0,1,ksize = 3, scale = scale, delta = delta, borderType = cv2.BORDER_DEFAULT)
    #grad_y = cv2.Scharr(gray,ddepth,0,1)

    abs_grad_x = cv2.convertScaleAbs(grad_x)   # converting back to uint8
    abs_grad_y = cv2.convertScaleAbs(grad_y)

    img = cv2.addWeighted(abs_grad_x,0.5,abs_grad_y,0.5,0)
    #dst = cv2.add(abs_grad_x,abs_grad_y)
    show(img)
    return img

# find rectangles from a thresholded image
def find_rectangles(thresh):
    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    areas = [ cv2.contourArea(c) for c in contours]
    indexes = range(len(contours))
    items = sorted(zip(indexes, areas, contours, hierarchy[0]), key=lambda x:x[0], reverse=True)
    return card_filter(items)

def card_filter(items):
    results = {}
    for index, area, contour, hierarchy in items:
        if area > MIN_CARD_AREA and area < MAX_CARD_AREA:
            #results.append([index, area, contour, hierarchy])
            results[index]=[index, area, contour, hierarchy]

    to_del = []
    for k, v in results.items():
        _, a, c, h = v
        if h[PARENT] in results:
            if a/results[h[PARENT]][1] > 0.9:
                to_del.append(h[PARENT])
            else:
                to_del.append(k)

    for i in set(to_del):
        results.pop(i)
    return [c for i,a,c,h in results.values()]

# @param: img - preprocced img
def extract_cards(img):
    pass

def draw_rectangle(img, contour):
    x,y,w,h = cv2.boundingRect(contour)
    cv2.rectangle(img, (x,y),(x+w,y+h),(0,255,0),1)


def draw_rect(img):
    thresh = preprocess(img)
    contours,hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    hierarchy = hierarchy[0]

    for component in zip(contours, hierarchy):
        currentContour = component[0]
        currentHierarchy = component[1]
        if not currentHierarchy[FIRST_CHILD] < 0:
            draw_rectangle(currentContour)
    show(img)

def test_draw_contour(filename):
    img_orig = cv2.imread(filename)
    for c in find_rectangles(preprocess(img_orig)):
        draw_rectangle(img_orig, c)

    show(img_orig)
    cv2.imwrite(os.path.join(os.path.dirname(filename), "contours_" + os.path.basename(filename)), img_orig)


def extract_cards(filename):
    img_orig = cv2.imread(filename)
    contours = find_rectangles(preprocess(img_orig))
    corners = [np.int0(cv2.cv.BoxPoints(cv2.minAreaRect(c))) for c in contours]
    card_corners=[sorted_corners(c) for c in corners if sortable(c)]
    cards = []
    for i, c1 in enumerate(card_corners):
        norm_card = np.array([[0, 0],[99,0],[99,139],[0,139]], dtype = np.float32)
        transform_matrix = cv2.getPerspectiveTransform(c1, norm_card)
        warp = cv2.warpPerspective(img_orig, transform_matrix, (100,140))
        cards.append(warp)
    return cards

def sorted_corners(corners):
    center = corners.sum(0) * 0.25

    top, bot = [], []
    for point in corners:
        if point[1] < center[1]:
            top.append(point)
        else:
            bot.append(point)

    if top[0][0] > top[1][0]:
        tl = top[1]
        tr = top[0]
    else:
        tl = top[0]
        tr = top[1]

    if bot[0][0] > bot[1][0]:
        bl = bot[1]
        br = bot[0]
    else:
        bl = bot[0]
        br = bot[1]

    return array([tl,tr,br,bl], dtype = np.float32)

def sortable(corners):
    center = corners.sum(0) * 0.25

    top, bot = [], []
    for point in corners:
        if point[1] < center[1]:
            top.append(point)
        else:
            bot.append(point)
    return len(top) == len(bot)


def main(filename, output_dir):
    cards = extract_cards(filename)
    for i, card in enumerate(cards):
        cv2.imwrite(os.path.join(output_dir,"full_card%02d.jpg" % i), card)

if __name__ == '__main__':
    main('../raw_images/desk_full1.jpg', '../cards')
    #test_draw_contour('../raw_images/desk_full1.jpg')
    #test_draw_contour('../raw_images/desk.jpg')

