#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vslam/position.h>
//#include <std_msgs/Float32.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
using namespace cv;
using namespace std;

ros::Publisher pub;
vslam::position cam_pos;
Point savePoint=Point(0,0);
int lenx=640;
int leny=480;
//存储某个坐标的信息，以后使用
struct myMap
{
    float angle;
    Point point;
    vector<KeyPoint> keypoint;
    Mat descriptors;
    
};
vector<myMap> mapVector;

struct simpleDate
{
    Point point;
    float angle;
};


struct sortAngle
{
    double angle;
    int index;
    
};

//计算反身旋转坐标系后的特征点坐标，以此计算位移
Point easyTf(Point p1,Point p2,double rad,int cols,int rows)
{
    p1.x-=cols/2;
    p1.y-=rows/2;
    p2.x-=cols/2;
    p2.y-=rows/2;
    Point p3;
    p3.x=round(p1.x-(p2.x*cos(rad)+p2.y*sin(rad)));
    p3.y=round(p1.y-(p2.y*cos(rad)-p2.x*sin(rad)));//左手坐标系，y取反
    //ROS_INFO("p1.x=%d,p2.x=%f",p1.x,p1.x-p2.x*cos(rad)+p2.y*sin(rad));
    //ROS_INFO("p1.y=%d,p2.y=%f",p1.y,p2.y*cos(rad)-p2.x*sin(rad));
    
    return p3;
}



//返回最小转向角
//比如右转270度转换成左转90
float minAngle(float angle)
{
    if (angle>180)return angle-360;
    else 
        if (angle<-180)return angle+360;
        else return angle;
}

//角度转弧度
double deg2rad(float angle)
{
    return angle*3.1415926/180;
}

int saveData(Point p,float angle)
{
    ofstream outFile;
    outFile.open("/home/lq/Pictures/point.txt",ios::app);
    if (!outFile.is_open()){
		cout << "open File Failed." << endl;
		return -1;
	}
    outFile << p.x<<"\t"<< p.y<<"\t"<<angle<<endl;
    outFile.close();
}
int readData(Point &p,float &angle)
{
    ifstream  inFile;
    inFile.open("/home/lq/Pictures/point.txt");
    if (!inFile.is_open()){
		cout << "open File Failed." << endl;
		return -1;
	}
    string strOne;
	while (getline(inFile, strOne)){
		stringstream ss;
		ss << strOne;
		ss >> p.x>> p.y>>angle;
	}

    inFile.close();
}
//自定义排序函数  
bool sortFun(const sortAngle &a1, const sortAngle &a2)
{
	return a1.angle < a2.angle;//升序排列  
}

//滑动窗口滤波后返回平均值
float windowFilter(vector<float> &v)
{
    //滑动窗口滤波
    sort(v.begin(),v.end());
    int wlen=(v.size()+2)/3;
    float minErr=10000.0;
    float minErrIdx=0;

    for(int i=0;i<v.size()-wlen;i++)
    {
        float err=v[i+wlen-1]-v[i];
        //ROS_INFO("acc=%f",accum);
        if(minErr>err)
        {
            minErr=err;
            minErrIdx=i;
        }
    }
    float sum=0;
    for(int i=minErrIdx;i<minErrIdx+wlen;i++)
        sum+=v[i];
    return sum/wlen;

}


int analyze(vector<KeyPoint> &keypoints1,Mat &descriptors1,vector<KeyPoint> &keypoints2,Mat &descriptors2,vector< DMatch > &good_matches, myMap &map)
{  
    //-- 第三步: 对两幅图像中的BRIEF描述子进行匹配,使用 Hamming 距离
    vector<DMatch> matches;
    BFMatcher matcher(NORM_HAMMING );
    matcher.match (descriptors1, descriptors2,matches);

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
    vector<sortAngle>radVector;
    
    for ( int i = 0; i < descriptors1.rows; i++ )
    {
        // if ( matches[i].distance <= max( 2*min_dist,43.0 ) )//5 效果好但要求特征明显，30角度可以，但位移不行
        // {
            good_matches.push_back ( matches[i] );
            sortAngle sa;
            sa.angle=minAngle(keypoints2[matches[i].trainIdx].angle-keypoints1[i].angle); 
            sa.index=i;
            radVector.push_back(sa);
            //ROS_INFO("rotate=%f",sa.angle);
        // }
    }



    //滑动窗口滤波
    
    if(radVector.size()==0)
    {
        ROS_ERROR("can not detect");
        return -1;
    }
    sort(radVector.begin(),radVector.end(),sortFun);
    for(int i=0;i<radVector.size();i++)
        ROS_INFO("rotate=%f",radVector[i].angle);

    int wlen=(radVector.size()+2)/3;
    float minErr=10000.0;
    
    float minErrIdx=0;

    
    for(int i=0;i<radVector.size()-wlen;i++)
    {
        float err=radVector[i+wlen-1].angle-radVector[i].angle;
        //ROS_INFO("rotate=%f",err);
        if(minErr>err)
        {
            minErr=err;
            minErrIdx=i;
        }
    }
    //ROS_INFO("rotate=%f",radVector[radVector.size()].angle-radVector[i].angle);

    //计算平均旋转角
    float rotateMean=0;
    for(int i=minErrIdx;i<minErrIdx+wlen;i++)
        rotateMean+=radVector[i].angle;
    rotateMean/=wlen;
    //ROS_INFO("acc=%f",minErr);


    for(int i=0;i<wlen;i++)
    {
        //good_matches.push_back(matches[radVector[minErrIdx+i].index]);
        //ROS_INFO("deg=%f,indx=%d",radVector[minErrIdx+i].angle*180/3.14159,radVector[minErrIdx+i].index);
    }
        
    map.angle+=rotateMean;//全局旋转角
    ROS_INFO("rotateAngle=%f,realAngle=%f",rotateMean,map.angle);

    //计算前后两个图的相对位移，其相对位移等于第二个图的坐标原点在第一个图的坐标
    Point tempPint=Point(0,0);
    for(int i=0;i<wlen;i++)
    {

        Point p=easyTf(keypoints1[good_matches[i].queryIdx].pt,keypoints2[good_matches[i].trainIdx].pt,deg2rad(rotateMean),lenx,leny);
        //ROS_INFO("(%.0lf,%.0lf),(%d,%d),%d",keypoints1[j].pt.x,keypoints1[j].pt.y,p.x,p.y,j);
        tempPint+=p;
    }

    Point drift=tempPint/wlen;//位移
    //把第二个图的坐标原点在第一个图的坐标转换成世界坐标
    map.point=easyTf(mapVector[mapVector.size()-1].point,drift,deg2rad(map.angle),0,0);;
    // map.keypoint.assign(keypoints2.begin(),keypoints2.end());//复制角点到kt
    // map.descriptors=descriptors2.clone();//复制描述子
    ROS_INFO("drift=(%d,%d),pos=(%d,%d)",drift.x,drift.y,map.point.x,map.point.y);

}




vector<KeyPoint> keypointsLast;
Mat descriptorLast;
vector<simpleDate>simplePose;
int voSystem(Mat img1,Mat img2)
{
    myMap map;
    if(img1.empty()||img2.empty())
    {
        ROS_ERROR("imread error");
        return -1;
    }

    //初始化
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    //Ptr<ORB> orb = ORB::create (500, 1.2f, 8, 91, 0, 2, ORB::HARRIS_SCORE,91,20 );//效果好
    Ptr<ORB> orb = ORB::create (500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE,31,20 );//即patchsize需要大，由于像素的不确定性，所以灰度质心法半径要大
    //-- 第一，二步: 检测 Oriented FAST 角点位置，根据角点位置计算 BRIEF 描述子
    orb->detect ( img1,keypoints1 );//检测 Oriented FAST 角点位置
    orb->detect ( img2,keypoints2 );
    if(keypoints2.size()==0)
    {
        ROS_ERROR("have no keypoint");
        return -1;
    }
    orb->compute ( img1, keypoints1, descriptors1 );//根据角点位置计算 BRIEF 描述子
    orb->compute ( img2, keypoints2, descriptors2 );

    //imwrite("/home/lq/Pictures/firstImg.jpg", img1);
    Mat outimg1;
    //drawKeypoints( img1, keypoints1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    //imshow("1",outimg1);
    
        //存储初始点数据
        
    map.keypoint.assign(keypoints1.begin(),keypoints1.end());//复制角点到kt
    keypointsLast.assign(keypoints1.begin(),keypoints1.end()); 

    map.descriptors=descriptors1.clone();//复制描述子
    descriptorLast=descriptors1.clone();

    map.point=Point(0,0);
    map.angle=0;
    mapVector.push_back(map);
    saveData(map.point, map.angle);


    int mapSize=mapVector.size();
    vector< DMatch > good_matches;
    analyze(keypoints1,descriptors1,keypoints2,descriptors2,good_matches,map);
    ROS_INFO("angle=%.2f,pos=(%d,%d)",map.angle,map.point.x,map.point.y);
    char text[100];
    //sprintf(text,"angle=%.2f,pos=(%d,%d)",map.angle,map.point.x,map.point.y);
    Mat img_goodmatch;
    drawMatches (img1, keypoints1, img2, keypoints2, good_matches, img_goodmatch);
    imwrite("/home/lq/Pictures/goodMatch.jpg", img_goodmatch);
    //putText(img_goodmatch, text,Point(600,30), cv::FONT_HERSHEY_TRIPLEX, 0.7, cv::Scalar(0, 255, 0), 1);
    imshow ("orb match", img_goodmatch);

    ROS_INFO("goodAngle=%.2f,goodPos=(%d,%d)",map.angle,map.point.x,map.point.y);
    //map.point=windowFilter(goodPoint);


    
    if((abs(map.angle-mapVector[mapSize-1].angle)>2&&abs(map.angle-mapVector[mapSize-1].angle)<10)
    ||(abs(map.point.x-mapVector[mapSize-1].point.x)>10&&abs(map.point.x-mapVector[mapSize-1].point.x<100))
    ||(abs(map.point.y-mapVector[mapSize-1].point.y)>10&&abs(map.point.y-mapVector[mapSize-1].point.y)<100))
    {
        map.keypoint.assign(keypoints2.begin(),keypoints2.end());//复制角点到kt
        map.descriptors=descriptors2.clone();//复制描述子
        mapVector.push_back(map);
        saveData(mapVector[mapSize-1].point,mapVector[mapSize-1].angle);//FPS=18 ,两分钟30kb
    }

    // vector< DMatch > tempMatches;
    // myMap tempMap;
    // simpleDate tempData;
    // analyze(keypointsLast,descriptorLast,keypoints2,descriptors2,tempMatches,tempMap);
    // keypointsLast.assign(keypoints2.begin(),keypoints2.end()); 
    // descriptorLast=descriptors2.clone();  
    // tempData.point=tempMap.point;
    // tempData.angle=tempData.angle;
    // simplePose.push_back(tempData);
    
    waitKey(10);
}

//回调函数
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    static cv::Mat imgPrior;
    static int imgCnt=0;
    try
    {
    
        if (imgCnt<1)
        {
            cam_pos.p.x=0;
            cam_pos.p.y=0;   
            cam_pos.angle=0;
            pub.publish(cam_pos);
            imgCnt++;
        }
        else
        {

            voSystem(imgPrior,cv_bridge::toCvShare(msg, "bgr8")->image);
            int dx=abs(mapVector[mapVector.size()-2].point.x-mapVector[mapVector.size()-1].point.x);
            int dy=abs(mapVector[mapVector.size()-2].point.y-mapVector[mapVector.size()-1].point.y);
            int da=abs(mapVector[mapVector.size()-2].angle-mapVector[mapVector.size()-1].angle);
            if(dx>0||dy>0||da>0.5)
            {
                cam_pos.p.x=mapVector[mapVector.size()-1].point.x;
                cam_pos.p.y=mapVector[mapVector.size()-1].point.y;   
                cam_pos.angle=mapVector[mapVector.size()-1].angle;
                //saveData(mapVector[mapVector.size()-1].point,cam_pos.angle);//FPS=18 ,两分钟30kb
            }
                
            pub.publish(cam_pos);
            
        }
        imgPrior=cv_bridge::toCvShare(msg, "bgr8")->image.clone();
    //cv::imshow("a", cv_bridge::toCvShare(msg, "bgr8")->image);
    //cv::waitKey(1);       
    
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}



int main(int argc, char** argv)
{
    ros::init(argc, argv, "record");
    ros::NodeHandle nh;//也有初始化node的作用
    //订阅主题，接收摄像头结点发来的图片
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("camera/image", 1, imageCallback);//camera/image   cv_camera/image_raw

    //创建publisher
    pub = nh.advertise<vslam::position>("position_info", 1);

    Mat img1 = imread("/home/lq/Pictures/empty1.jpg", CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE灰度
    Mat img2 = imread("/home/lq/Pictures/empty2.jpg",CV_LOAD_IMAGE_COLOR );
    voSystem(img1,img2);

    //voSystem();
    
    ros::Rate loop_rate(0.5);
    while (ros::ok())
    {
        voSystem(img1,img2);
        //ros::spin();
        loop_rate.sleep();
    }
    
    return 0;
}