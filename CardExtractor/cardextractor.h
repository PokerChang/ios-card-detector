//
//  cardextractor.h
//  CardExtractor
//
//  Created by Young Ng on 8/3/13.
//  Copyright (c) 2013 Young Ng. All rights reserved.
//

#ifndef CardExtractor_cardextractor_h
#define CardExtractor_cardextractor_h

#define MAX_CARD_AREA 50000.0
#define MIN_CARD_AREA 10000.0
#define MIN_CARD_CONTOUR_SIZE 10

#define HIERARCHY_NEXT 0
#define HIERARCHY_PREV 1
#define HIERARCHY_FIRST_CHILD 2
#define HIERARCHY_PARENT 3

#define NORMALIZE_CARD_WIDTH 100
#define NORMALIZE_CARD_HEIGHT 140


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

typedef struct CardContour {
    size_t index;
    double area;
    vector<Point> contour;
    Vec4i hierarchy;
} CardContour;

void showImage(const Mat& img);

// preproccess given image
void preproccess(const Mat& src, Mat& dst);

// find rectangels from a preproccessed image
void findRectangles(const Mat& thresh, vector<vector<Point>>& rects);

void filterCards();

/*
 * input original image (size <= 1280x1024)
 * output array of warp objects
 */
void extractCards(const Mat& img, vector<Mat>& cards);
void sortedCorners(const vector<Point>& contour, vector<Point2f>& corners);
bool compareY(const Point2f& p1, const Point2f& p2);

void drawRectangles(Mat& img, vector<vector<Point>>& contours);

#endif
