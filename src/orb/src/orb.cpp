#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <cmath>
using namespace cv;
using namespace std;

struct indexPoint
{
    Point point;
    int index;
    
};
struct sortAngle
{
    double angle;
    int index;
    
};

//计算反身旋转坐标系后的特征点坐标，以此计算位移
Point aculatePoint(Point p1,Point p2,double rad,int cols,int rows)
{
    p1.x-=cols/2;
    p1.y-=rows/2;
    p2.x-=cols/2;
    p2.y-=rows/2;
    Point p3;
    p3.x=round(p1.x-(p2.x*cos(rad)+p2.y*sin(rad)));
    p3.y=-round(p1.y-(p2.y*cos(rad)-p2.x*sin(rad)));//不是右手坐标系
    //ROS_INFO("p1.x=%d,p2.x=%f",p1.x,p1.x-p2.x*cos(rad)+p2.y*sin(rad));
    //ROS_INFO("p1.y=%d,p2.y=%f",p1.y,p2.y*cos(rad)-p2.x*sin(rad));
    
    return p3;
}


//返回最小转向角,
//比如右转270度转换成左转90
float minAngle(float angle)
{
    if (angle>180)
        return angle-360;
    else 
        if (angle<-180)return angle+360;
        else return angle;
}

//返回最小转向角
//比如右转270度转换成左转90
float minRad(float rad)
{
    if (rad>3.1415926)
        return rad-2*3.1415926;
    else 
        if (rad<-3.1415926)return rad+2*3.1415926;
        else return rad;
}

//角度转弧度
double deg2rad(float angle)
{
    return angle*3.1415926/180;
}


//存储某个坐标的信息，以后使用
struct myMap
{
    Point pt;
    vector<KeyPoint> kt;
    Mat descriptors;
};

//自定义x排序函数  
bool sortFun(const sortAngle &a1, const sortAngle &a2)
{
	return a1.angle < a2.angle;//升序排列  
}
//自定义y排序函数  
bool sortyFun(const indexPoint &p1, const indexPoint &p2)
{
	return p1.point.y < p2.point.y;//升序排列  
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_publisher");
    ros::NodeHandle nh;//也有初始化node的作用
    Mat img1 = imread("/home/lq/Pictures/empty1.jpg", CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE灰度
    Mat img2 = imread("/home/lq/Pictures/empty2.jpg",CV_LOAD_IMAGE_COLOR );
    myMap map;
    int lenx=img1.cols;
    int leny=img1.rows;
    if(img1.empty()||img2.empty())
    {
        ROS_INFO("imread error");
        return -1;
    }

    cv::TickMeter tm;
   tm.reset();
   tm.start();
    //初始化
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    Ptr<ORB> orb = ORB::create (500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE,31,20 );
    //-- 第一步: 检测 Oriented FAST 角点位置
    orb->detect ( img1,keypoints1 );
    orb->detect ( img2,keypoints2 );
    //-- 第二步: 根据角点位置计算 BRIEF 描述子
    orb->compute ( img1, keypoints1, descriptors1 );
    orb->compute ( img2, keypoints2, descriptors2 );
    map.kt.assign(keypoints1.begin(),keypoints1.end());//复制角点到kt
    map.descriptors=descriptors1.clone();//复制描述子
    // Mat outimg1;
    // drawKeypoints( img1, keypoints1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    // imshow("ORB",outimg1);

    //-- 第三步: 对两幅图像中的BRIEF描述子进行匹配,使用 Hamming 距离
    vector<DMatch> matches;
    BFMatcher matcher(NORM_HAMMING );
    matcher.match (descriptors1, descriptors2,matches);
    ROS_INFO("cnt=%ld",matches.size());
    tm.stop();
    ROS_INFO("orb_time=%lf",tm.getTimeSec());
    //-- 第四步:匹配点对筛选，并取中间三分这部分计算旋转散席
    double min_dist=10000;
    int minDex=0;
    // 找出所有匹配之间的最小距离
    for ( int i = 0; i < descriptors1.rows; i++ )
    {
        double dist = matches[i].distance;
        if ( dist < min_dist ) 
        {
            min_dist = dist;
            minDex=i;
        }
    }
    // 当描述子之间的距离大于两倍的最小距离时,即认为匹配有误。
    // 但有时候最小距离会非常小,设置一个经验值作为下限。
    std::vector< DMatch > good_matches;
    std::vector< float > angleVector;
    vector< int > indexVector;//存储计算角度用的特征点的索引 ，用于计算坐标
    for ( int i = 0; i < descriptors1.rows; i++ )
    {
        //ROS_INFO("px=%f,py=%f",keypoints2[i].pt.x,keypoints2[i].pt.y);
        if ( matches[i].distance <= max( 2*min_dist, 3.0 ) )
        {
            good_matches.push_back ( matches[i] );
            Point p1=keypoints1[i%descriptors1.rows].pt-keypoints1[(i+1)%descriptors1.rows].pt;
            Point p2=keypoints2[matches[i%descriptors1.rows].trainIdx].pt-keypoints2[matches[(i+1)%descriptors1.rows].trainIdx].pt;
            //记录最小转向角
            angleVector.push_back(minAngle(keypoints2[matches[i].trainIdx].angle-keypoints1[i].angle));
            //float test=atan()
            ROS_INFO("1=%f,2=%f", minAngle((atan2(p2.y,p2.x)-atan2(p1.y,p1.x))*180/3.14159),minAngle(keypoints2[matches[i].trainIdx].angle-keypoints1[i].angle));
            indexVector.push_back(i);
        }
    }

    int gcnt=good_matches.size();
    double sumRad=0;
    vector<sortAngle>radVector;
    for(int i=0;i<gcnt;i++)
    {
        for(int j=i+1;j<gcnt;j++)
        {
            Point p1=keypoints1[good_matches[i].queryIdx].pt-keypoints1[good_matches[j].queryIdx].pt;
            Point p2=keypoints2[good_matches[i].trainIdx].pt-keypoints2[good_matches[j].trainIdx].pt;
            sortAngle sa;
            sa.angle=minRad(atan2(p2.y,p2.x)-atan2(p1.y,p1.x));
            sa.index=i;
            radVector.push_back(sa);

            ROS_INFO("deg=%f,rad=%f", sa.angle*180/3.14159,sa.angle);
            sumRad+=sa.angle;
        //ROS_INFO("1=%f", atan2(p2.y,p2.x)-atan2(p1.y,p1.x));
        }
        //Point p1=keypoints1[good_matches[i%gcnt].queryIdx].pt-keypoints1[good_matches[(i+1)%gcnt].queryIdx].pt;
        //Point p2=keypoints2[good_matches[i%gcnt].trainIdx].pt-keypoints2[good_matches[(i+1)%gcnt].trainIdx].pt;
        //sumRad+=minAngle((atan2(p2.y,p2.x)-atan2(p1.y,p1.x));
      // ROS_INFO("1=%f,2=%f", minAngle((atan2(p2.y,p2.x)-atan2(p1.y,p1.x))*180/3.14159),minAngle(keypoints2[good_matches[i].trainIdx].angle-keypoints1[good_matches[i].queryIdx].angle));
       // ROS_INFO("1=%f", atan2(p2.y,p2.x)-atan2(p1.y,p1.x));
    }
    sort(radVector.begin(),radVector.end(),sortFun);
    int wlen=radVector.size()/3;
    double minAcc=10000;
    int minAccIdx=0;
    for(int i=0;i<radVector.size()-wlen;i++)
    {
        double sum=0;
        for(int j=0;j<wlen;j++)
            sum+=radVector[i+j].angle;
        double mean=sum/wlen;

        double accum=0;
        for(int j=0;j<wlen;j++)
        {
            accum+=(radVector[i+j].angle-mean)*(radVector[i+j].angle-mean);
            
        }
        ROS_INFO("acc=%f",accum);
        if(minAcc>accum)
        {
            minAcc=accum;
            minAccIdx=i;
        }
    }
    for(int i=0;i<wlen;i++)
        ROS_INFO("deg=%f,ind=%d",radVector[minAccIdx+i].angle*180/3.14159,minAccIdx);
        

    // double mean =  sumRad/radVector.size(); //均值
    // double accum  = 0.0;
    // for(int i=0;i<radVector.size();i++)
	// 	accum  += (radVector[i]-mean)*(radVector[i]-mean);//方差
 
    // double stdev = sqrt(accum/(radVector.size()-1)); //标准差
    // ROS_INFO("mean=%f,stdev=%f",mean,stdev);
    // for(int i=0;i<radVector.size();i++)
    // {
    //     static int t=0;
    //     // if(radVector[i]>=mean-3*stdev&&radVector[i]<=mean+3*stdev)
    //     //     ROS_INFO("deg=%f,int=%d", radVector[i]*180/3.14159,t++);
            
    // }

    
    

    //排序，取中间三分之一的数据取平均作为旋转角    
    sort(angleVector.begin(), angleVector.end());
    int aculateCnt=angleVector.size()/3;
    float rotateAngle=0;
    for(int i=aculateCnt;i<=aculateCnt*2;i++)
    {
        rotateAngle+=angleVector[i];
        //good_matches.push_back (matches[indexVector[i]]);
        //ROS_INFO("one=%f",minAngle(keypoints2[matches[indexVector[i]].trainIdx].angle-keypoints1[indexVector[i]].angle));
    }
        

    rotateAngle/=(1+aculateCnt);
    double radAngle=deg2rad(rotateAngle);
    ROS_INFO("angleCnt=%d,angle=%f",aculateCnt+1,rotateAngle);


    //显示匹配图
    Mat img_goodmatch;
    drawMatches (img1, keypoints1, img2, keypoints2, good_matches, img_goodmatch );
    imshow ("orb match", img_goodmatch);

     vector<indexPoint> driftVector;//分别对x,y进行排序，取中间部分
    //计算平均位移
    Point tempPoint=Point(0,0);
    for(int i=aculateCnt;i<=aculateCnt*2;i++)
    {
        int j=indexVector[i];
        Point p=aculatePoint(keypoints1[j].pt,keypoints2[matches[j].trainIdx].pt,radAngle,lenx,leny);
        //ROS_INFO("(%.0lf,%.0lf),(%d,%d),%d",keypoints1[j].pt.x,keypoints1[j].pt.y,p.x,p.y,j);
         tempPoint+=p;
        // sumy+=p.y;
    }
    // sumx/=1+aculateCnt;
    // sumy/=1+aculateCnt;
    map.pt=tempPoint/(1+aculateCnt);
    ROS_INFO("pos=(%d,%d)",map.pt.x,map.pt.y);
    waitKey();
    
    




  // Mat t;
  // t.create();


  // ros::Rate loop_rate(0.3);
  // while (ros::ok())
  // {
  //   ROS_INFO("wait");

  //   loop_rate.sleep();//根据前面的定义的loop_rate,设置1s的暂停
  // }
    return 0;


}