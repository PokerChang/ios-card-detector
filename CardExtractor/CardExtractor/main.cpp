//
//  main.cpp
//  CardExtractor
//
//  Created by Young Ng on 8/3/13.
//  Copyright (c) 2013 Young Ng. All rights reserved.
//

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "cardextractor.h"

using namespace cv;
using namespace std;



int main( int argc, char** argv )
{
    if( argc != 2)
    {
        cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }
    
    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    
    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    vector<Mat> cards;
    extractCards(image, cards);
    
    for(size_t i=0; i < cards.size(); i++){
        cout<<"in main for cards "<< i <<endl;
        showImage(cards[i]);
    }

    return 0;
}


