//
//  ViewController.m
//  TestOpenCV
//
//  Created by Chang Luo on 5/11/13.
//  Copyright (c) 2013 Chang Luo. All rights reserved.
//

#import "ViewController.h"
#import <opencv2/highgui/cap_ios.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp" //This is where actual SURF and SIFT algorithm is located

using namespace cv;

@interface ViewController () <CvVideoCameraDelegate>{
    std::vector<KeyPoint> keypoints_object, keypoints_scene;
    SiftDescriptorExtractor extractor;
    Mat descriptors_object;


}
@property (nonatomic, retain) CvVideoCamera* videoCamera;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
//    self.videoCamera = [[CvVideoCamera alloc] initWithParentView:self.imageView];
//    self.videoCamera.delegate = self;
//    self.videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
//    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
//    self.videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
//    self.videoCamera.defaultFPS = 30;
//    self.videoCamera.grayscaleMode = YES;
//    [self.videoCamera start];
    NSString *path = [[NSBundle mainBundle] pathForResource:@"AceOfDiamond" ofType:@"bmp"];
    const char * cpath = [path cStringUsingEncoding:NSUTF8StringEncoding];
    
    Mat img_object = imread(cpath, CV_LOAD_IMAGE_UNCHANGED );
    
    if( !img_object.data)
    {
        NSLog(@"no object image");
    }
    SiftFeatureDetector detector(1);
    detector.detect( img_object, keypoints_object );
    extractor.compute( img_object, keypoints_object, descriptors_object );
    NSLog(@"object keypoints %ld", keypoints_object.size());
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.

}

- (IBAction)actionStart:(id)sender;
{
}

#pragma mark - Protocol CvVideoCameraDelegate

#ifdef __cplusplus
- (void)processImage:(Mat&)img_scene;
{
    //-- Step 1: Detect the keypoints using SURF Detector
    SurfFeatureDetector detector(5);
    detector.detect( img_scene, keypoints_scene );
    
    //-- Step 2: Calculate descriptors (feature vectors)
    
    Mat descriptors_scene;
    
    extractor.compute( img_scene, keypoints_scene, descriptors_scene );
    
    //-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );
    
    double max_dist = 0; double min_dist = 100;
    
    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_object.rows; i++ )
    { double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    
    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );
    
    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< DMatch > good_matches;
    
    for( int i = 0; i < descriptors_object.rows; i++ )
    { if( matches[i].distance < 3*min_dist )
    { good_matches.push_back( matches[i]); }
    }
    NSLog(@"good matches:%ld", good_matches.size());
}
#endif

@end
