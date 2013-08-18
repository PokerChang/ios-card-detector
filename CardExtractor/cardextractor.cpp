//
//  cardextractor.cpp
//  CardExtractor
//
//  Created by Young Ng on 8/3/13.
//  Copyright (c) 2013 Young Ng. All rights reserved.
//



#include "cardextractor.h"
#include "contains.h"
#include <set>
#include <map>

using namespace cv;
using namespace std;


void showImage(const Mat& img){
    namedWindow( "Image", CV_WINDOW_AUTOSIZE );
    imshow("Image", img);
    waitKey(0);
}

// preproccess given image
void preproccess(const Mat& src, Mat& dst){
    Mat gray, blur, thresh;
    cvtColor(src, gray, CV_RGB2GRAY);
    GaussianBlur(gray, blur, Size(5, 5), 0);
    adaptiveThreshold(blur, thresh, 255, 1, 1, 11, 2);
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(thresh, dst, MORPH_CLOSE, element);
}

// find rectangels from a preproccessed image
void findRectangles(const Mat& thresh, vector<vector<Point>>& rects){
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    

    map<size_t, CardContour> ccMap;
    
    for(size_t i=0; i<contours.size(); i++){
        vector<Point> contour = contours[i];
        Vec4i contourHierachy = hierarchy[i];
        // filter by contour size (points)
        if(contourHierachy[HIERARCHY_FIRST_CHILD] == -1 || contour.size() < MIN_CARD_CONTOUR_SIZE){
            continue;
        }
        // filter by contour area
        double area = contourArea(contour);
        if(area > MAX_CARD_AREA || area < MIN_CARD_AREA){
            continue;
        }

        CardContour cc = {i, area, contour, contourHierachy};
        ccMap.insert(make_pair(i, cc));
    }
    
    set<size_t> toDel;
    map<size_t, CardContour>::iterator iter1;
	for (iter1 = ccMap.begin(); iter1 != ccMap.end(); ++iter1){
        size_t parentIndex = iter1->second.hierarchy[HIERARCHY_PARENT];
        map<size_t, CardContour>::iterator parent = ccMap.find(parentIndex);
        if(parent!=ccMap.end()){
            // found
            double area = iter1->second.area;
            double parentArea = parent->second.area;
            if(area/parentArea > 0.9){
                toDel.insert(parentIndex);
            }else{
                toDel.insert(iter1->first);
            }
        }
        
    }
    
    set<size_t>::iterator iter2;
    for(iter2 = toDel.begin(); iter2 != toDel.end(); ++iter2){
        ccMap.erase(*iter2);
    }
    
    for (iter1 = ccMap.begin(); iter1 != ccMap.end(); ++iter1){
        rects.push_back(iter1->second.contour);
    }
    
}

void filterCards(){
    
}

/*
 * input original image (size <= 1280x1024)
 * output array of warp objects
 */
void extractCards(const Mat& img, vector<Mat>& cards){
    Mat thresh;
    // preproccess to generate a thresholding image
    preproccess(img, thresh);
    
    vector<vector<Point>> contours;
    // find rectangles threshhold
    findRectangles(thresh, contours);
    
    for(size_t i=0; i<contours.size(); i++){
        Mat card = Mat(NORMALIZE_CARD_HEIGHT, NORMALIZE_CARD_WIDTH,   CV_8UC3);
        vector<Point2f> corners;
        sortedCorners(contours[i], corners);
        
        vector<Point2f> pts;
        pts.push_back(Point2f(0, 0));
        pts.push_back(Point2f(card.cols, 0));
        pts.push_back(Point2f(card.cols, card.rows));
        pts.push_back(Point2f(0, card.rows));
        
        Mat transMx = getPerspectiveTransform(corners, pts);
        warpPerspective(img, card, transMx, card.size());
        cards.push_back(card);
    }
    
}

bool compareY(const Point2f& p1, const Point2f& p2){
    return p1.y < p2.y;
}

/**
 *  output corners ( if the given contour is not rect, corners will be empty)
 */
void sortedCorners(const vector<Point>& contour, vector<Point2f>& corners){
    corners.clear();
    Point2f points[4];
    minAreaRect(contour).points(points);

    for(int i=0; i<4; i++){
        corners.push_back(points[i]);
    }
    
    sort(corners.begin(), corners.end(), compareY);
    
    Point2f tmp;
    
    if(corners[0].x > corners[1].x){
        tmp = corners[0];
        corners[0] = corners[1];
        corners[1] = tmp;
    }
    
    if(corners[2].x < corners[3].x){
        tmp = corners[2];
        corners[2] = corners[3];
        corners[3] = tmp;
    }
    
}


// for testing only
void drawRectangles(const Mat& img, vector<vector<Point>>& contours){
    Mat tmpImg;
    img.copyTo(tmpImg);
    for(size_t i=0; i<contours.size(); i++){
        Rect rect = boundingRect(contours[i]);
        rectangle(tmpImg, rect,  Scalar(0,255,0));
    }
    showImage(tmpImg);
}

// for testing only
void drawCorners(const Mat& img, vector<vector<Point>>& contours){
    Mat tmpImg;
    img.copyTo(tmpImg);
    for(size_t i=0; i < contours.size(); i++){
        vector<Point2f> corners;
        sortedCorners(contours[i], corners);
        for(int j=0; j<corners.size(); j++){
            circle(tmpImg, corners[j], 3, Scalar(0,255,0));
        }
        
    }
    showImage(tmpImg);
}