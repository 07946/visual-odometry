#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp> // for homography
#include <opencv2/opencv_modules.hpp>
#include <opencv2/xfeatures2d.hpp>
using namespace cv;
using namespace std;

int sift(Mat img1,Mat img2)
{
    if(img1.empty()||img2.empty())
    {
        ROS_ERROR("imread error");
        return -1;
    }
        
   cv::TickMeter tm;
   tm.reset();
   tm.start();

    cv::Ptr<cv::FeatureDetector> detector;

    // detector = cv::xfeatures2d::SURF::create();
    detector = cv::xfeatures2d::SIFT::create();
    std::vector<cv::KeyPoint> objectKeypoints;
	std::vector<cv::KeyPoint> sceneKeypoints;
    
	cv::Mat objectDescriptors;
	cv::Mat sceneDescriptors;
    detector->detect(img1, objectKeypoints);
    detector->detect(img2, sceneKeypoints);

    Mat outimg1;
    drawKeypoints( img1, objectKeypoints, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    imwrite("/home/lq/Pictures/siftn.jpg", outimg1);
    cv::Ptr<cv::DescriptorExtractor> extractor;
    extractor = cv::xfeatures2d::SIFT::create();
    extractor->compute(img1, objectKeypoints, objectDescriptors);
    extractor->compute(img2, sceneKeypoints, sceneDescriptors);
    //Mat outimg2;
    //drawKeypoints( img1, objectKeypoints, outimg2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    //drawKeypoints( img2, sceneKeypoints, outimg2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    //imshow("1",outimg2);
    


    vector<DMatch> matches;
    Mat matchImage;
    //FlannBasedMatcher matcher;
    BFMatcher matcher(cv::NORM_L2);
    matcher.match (objectDescriptors, sceneDescriptors,matches);

    tm.stop();
    ROS_INFO("sift_time=%lf",tm.getTimeSec());

    ROS_INFO("%ld",matches.size());

    drawMatches(img1, objectKeypoints, img2, sceneKeypoints, matches, matchImage);
    imwrite("/home/lq/Pictures/siftImg.jpg", matchImage);
    imshow("Match", matchImage);
    // BFMatcher matcher(cv::NORM_L2);
    // vector<DMatch>matches;
    // matcher.match(objectDescriptors, sceneDescriptors, matches);
    
     waitKey(1);




}




int main(int argc, char** argv)
{
    ros::init(argc, argv, "siftTest");
    ros::NodeHandle nh;//也有初始化node的作用

    Mat img1 = imread("/home/lq/Pictures/lenna1.bmp", CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE灰度
    Mat img2 = imread("/home/lq/Pictures/lenna2.bmp",CV_LOAD_IMAGE_COLOR );

    //voSystem();
    cv::TickMeter tm;
    ros::Rate loop_rate(1);
    while (ros::ok())
    {
        tm.reset();
        tm.start();
        sift(img1,img2);
        loop_rate.sleep();
        tm.stop();
        //ros::spin();
        
    }
    
    return 0;
}