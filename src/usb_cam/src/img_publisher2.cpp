#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <fstream>

 
int main(int argc, char** argv)
{
  ros::init(argc, argv, "img_publisher2");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image", 1);
  
  cv::Mat frame;
  ros::Rate loop_rate(1);//虽然要120帧，但还取决于摄像头本身帧率
  

  ROS_INFO("ready to publish");
  cv::TickMeter tm;

  int tempCnt=0;
  bool cg=true;
  cv::Mat img1 = cv::imread("/home/lq/Pictures/empty3.jpg", CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE灰度
  cv::Mat img2 = cv::imread("/home/lq/Pictures/empty4.jpg",CV_LOAD_IMAGE_COLOR );

  while (nh.ok())
  {
    tempCnt++;
    tm.reset();
	tm.start();
    sensor_msgs::ImagePtr msg;
	if(cg)
    {
      
		   msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img1).toImageMsg();

  	}
    else
    {    
		  msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img2).toImageMsg();
    }
    //if(tempCnt<10)
	pub.publish(msg);
    
    tm.stop();
    ROS_INFO("fps=%.0f,cols=%d,rows=%d",1/tm.getTimeSec(),frame.cols,frame.rows);//显示帧率
    cg=!cg;
    loop_rate.sleep();


    //以下为测试用
    // static int i=1;
    // for(;i<=4;i++)
    // {
    //   char str[50];
    //   sprintf(str,"/home/lq/Pictures/lenna%d.bmp",i);
    //   cv::Mat img =cv::imread(str,CV_LOAD_IMAGE_COLOR);
    //   sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img).toImageMsg();
    //   loop_rate.sleep();
		//   pub.publish(msg);
    // } 
  }
  return 0;
}