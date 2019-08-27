#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <fstream>

 
int main(int argc, char** argv)
{
  ros::init(argc, argv, "img_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image", 1);
  
  cv::VideoCapture cap;
  cv::Mat frame;
  int deviceID=0;
  if(argc>1)
	deviceID=argv[1][0]-'0';
  int apiID=cv::CAP_ANY;//自动搜索ID
  cap.open(deviceID+apiID);
  if(!cap.isOpened()){
	  ROS_ERROR("camera open err");
	  return -1;
  }

  //只能使用摄像头支持的几种分辨率
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
  
 
  ros::Rate loop_rate(18);//虽然要120帧，但还取决于摄像头本身帧率
	
  ROS_INFO("ready to publish");
  cv::TickMeter tm;

  int tempCnt=0;


  while (nh.ok())
  {
    tempCnt++;
    tm.reset();
		tm.start();
	  cap.read(frame);//获取每一帧
	  if(!frame.empty())
    {
      
		  sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
      //if(tempCnt<10)
		    pub.publish(msg);
        //loop_rate.sleep();
        tm.stop();
        ROS_INFO("fps=%.0f,cols=%d,rows=%d",1/tm.getTimeSec(),frame.cols,frame.rows);//显示帧率
  	}


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