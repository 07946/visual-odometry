#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <random>
#include<algorithm>
using namespace cv;
using namespace std;

Point2f O;
double R;
double sqr(double x){return x*x;}
double dis(Point2f p1,Point2f p2)
{
    return sqrt(sqr(p1.x-p2.x)+sqr(p1.y-p2.y));
}

bool incircle(Point2f p)
{
    if(dis(O,p)<=R) return true;
    return false;
}

Point2f solve(double a,double b,double c,double d,double e,double f)
{
    Point2f p;
    p.y=(f*a-c*d)/(b*d-e*a);
    p.x=(f*b-c*e)/(a*e-b*d);
    return p;
}

double analyzeR(vector<Point2f> vp)
{
    int n=vp.size();
    
    random_shuffle(vp.begin(),vp.end());

    R=0;
    for(int i=0;i<n;i++)
    if(!incircle(vp[i]))
    {
        O.x=vp[i].x;O.y=vp[i].y;R=0;
        for(int j=0;j<i;j++)
        if(!incircle(vp[j]))
        {
            O.x=(vp[i].x+vp[j].x)/2;
            O.y=(vp[i].y+vp[j].y)/2;
            R=dis(O,vp[i]);
            for(int k=0;k<j;k++)
            if(!incircle(vp[k]))
            {
                O=solve(
                vp[i].x-vp[j].x,vp[i].y-vp[j].y,(sqr(vp[j].x)+sqr(vp[j].y)-sqr(vp[i].x)-sqr(vp[i].y))/2,
                vp[i].x-vp[k].x,vp[i].y-vp[k].y,(sqr(vp[k].x)+sqr(vp[k].y)-sqr(vp[i].x)-sqr(vp[i].y))/2 
                );
                R=dis(vp[i],O);
            }
        }
    }
    ROS_INFO("%.10lf\n%.10lf %.10lf",R,O.x,O.y);
    return R;
    //ROS_INFO("R=%.3lf,P(%.3lf %.3lf)",R,O.x,O.y);
}










int main(int argc, char** argv)
{
    ros::init(argc, argv, "record");
    ros::NodeHandle nh;//也有初始化node的作用

    Mat img1 = imread("/home/lq/Pictures/empty1.jpg", CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE灰度
    Mat img2 = imread("/home/lq/Pictures/empty2.jpg",CV_LOAD_IMAGE_COLOR );

    cv::TickMeter tm;

    vector<Point2f> vp;
    vp.push_back(Point2f(0,0));
    vp.push_back(Point2f(0,1.0));
    vp.push_back(Point2f(1.0,0));
    vp.push_back(Point2f(0,-1.0));
    vp.push_back(Point2f(0,0.7));
    vp.push_back(Point2f(0.2,0.2));
    ros::Rate loop_rate(1);
    while (ros::ok())
    {
        tm.reset();
        tm.start();
        analyzeR(vp);
        loop_rate.sleep();
        tm.stop();
        ROS_INFO("Time=%lf",tm.getTimeSec());
        //ros::spin();
        
    }
    
    return 0;
}