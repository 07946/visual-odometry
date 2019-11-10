#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vslam/position.h>
//#include <std_msgs/Float32.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <random>
#include<algorithm>
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
    vector<KeyPoint> keypoint;
    Mat descriptors;
    Mat img;
    
};
vector<myMap> mapVector;

struct linkData
{
    int head;
    int tail;
    float gloabA;
    Point gloabP;
    int  score;
};


struct sortData
{
    double data;
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
bool sortFun(const sortData &a1, const sortData &a2)
{
	return a1.data < a2.data;//升序排列  
}


//滑动窗口滤波后返回索引
int windowFilter(vector<sortData> &v,int wlen)
{
    float minErr=10000.0;
    float minErrIdx=0;

    for(int i=0;i<=v.size()-wlen;i++)
    {
        float err=v[i+wlen-1].data-v[i].data;
        //ROS_INFO("acc=%f",accum);
        if(minErr>err)
        {
            minErr=err;
            minErrIdx=i;
        }
    }
    return minErrIdx;
}


void selsectMatches(vector<DMatch> &match1,vector<DMatch> &match2)
{
        //匹配点对筛选
        double min_dist=10000;
        // 找出所有匹配之间的最小距离
        for ( int i = 0; i < match1.size(); i++ )
        {
            if ( match1[i].distance < min_dist ) 
            {
                min_dist = match1[i].distance;
            }
        }
        // 当描述子之间的距离大于两倍的最小距离时,即认为匹配有误。
        // 但有时候最小距离会非常小,设置一个经验值作为下限。
        for ( int i = 0; i < match1.size(); i++ )
        {
            if ( match1[i].distance <= max( 2*min_dist,50.0 ) )//5 效果好但要求特征明显，30角度可以，但位移不行
            {
                match2.push_back(match1[i]);
            }
        }

}






vector<linkData>fr1;
vector<linkData>fr2;

vector<DMatch>matches12;
vector<DMatch>smatches12;



int voSystem(Mat img)
{  
    myMap map;
    Mat img1;
    Mat img2;
    BFMatcher matcher(NORM_HAMMING );

    linkData ld;
    ld.gloabA=0;
    ld.gloabP=Point(0,0);
    ld.head=-1;
    ld.tail=-1;
    ld.score=0;
    if(img.empty())
    {
        ROS_ERROR("imread error");
        return -1;
    }

    cv::TickMeter tm;
    tm.reset();
	tm.start();

    Ptr<ORB> orb = ORB::create (500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE,31,20 );
    orb->detect (img,map.keypoint);
    if(map.keypoint.size()==0)
    {
        ROS_ERROR("have no keypoint");
        return -1;
    }
    orb->compute (img,map.keypoint, map.descriptors);//根据角点位置计算 BRIEF 描述子
    map.img=img.clone();
    mapVector.push_back(map);

    int mapSize=mapVector.size();
    if(mapSize==2)
    {
        matcher.match (mapVector[0].descriptors, mapVector[1].descriptors,matches12);
        selsectMatches(matches12,smatches12);

        for(int i=0;i<mapVector[0].keypoint.size();i++)
            fr1.push_back(ld);
        for(int i=0;i<mapVector[1].keypoint.size();i++)
            fr2.push_back(ld);

        for(int i=0;i<smatches12.size();i++)
        {
            fr1[smatches12[i].queryIdx].tail=smatches12[i].trainIdx;
            fr2[smatches12[i].trainIdx].head=smatches12[i].queryIdx;
        }
    }
    
    if(mapVector.size()>=3)
    {


        img1=mapVector[0].img.clone();
        img2=mapVector[1].img.clone();

        vector<DMatch>matches23;
        vector<DMatch>smatches23;
        vector<DMatch>matches31;
        vector<DMatch>smatches31; 
        vector<linkData>fr3;
        matcher.match (mapVector[1].descriptors, mapVector[2].descriptors,matches23);
        selsectMatches(matches23,smatches23);

        matcher.match (mapVector[2].descriptors, mapVector[0].descriptors,matches31);
        selsectMatches(matches31,smatches31);
        //赋链接
        for(int i=0;i<mapVector[2].keypoint.size();i++)
            fr3.push_back(ld);
        for(int i=0;i<smatches23.size();i++)
        {
            fr2[smatches23[i].queryIdx].tail=smatches23[i].trainIdx;
            fr3[smatches23[i].trainIdx].head=smatches23[i].queryIdx;
        }


        int cnt_triple=0;
        //筛选三连匹配点
        for(int i=0;i<smatches31.size();i++)
        {
            
            if(fr1[smatches31[i].trainIdx].tail!=-1 && smatches31[i].queryIdx==fr2[fr1[smatches31[i].trainIdx].tail].tail)
            {                          
                //筛选三连点
                fr3[smatches31[i].queryIdx].score++;
                fr2[fr1[smatches31[i].trainIdx].tail].score++;
                fr1[smatches31[i].trainIdx].score++;
                cnt_triple++;
            }
        
        }
        ROS_INFO("cnt_triple=%d",cnt_triple);

        vector<sortData>radVector;
        for(int i=0;i<mapVector[0].keypoint.size();i++)
        {
            if(fr1[i].score==1)
            {
                sortData sa;
                sa.data=minAngle(mapVector[1].keypoint[fr1[i].tail].angle-mapVector[0].keypoint[i].angle); 
                sa.index=i;
                radVector.push_back(sa);
            }
        }

        //滑动窗口滤波
        ROS_INFO("cnt_radVector=%d",radVector.size());
        if(radVector.size()==0)
        {
            ROS_ERROR("can not detect");
            return -1;
        }
        sort(radVector.begin(),radVector.end(),sortFun);
        // for(int i=0;i<radVector.size();i++)
        //     ROS_INFO("rotate=%f",radVector[i].data);

        int wlen=(radVector.size()+1)/2;//加2是为了比如只有1个特征点，长度为1
        float minErrIdx=windowFilter(radVector,wlen);

        vector< DMatch > goodA_matches;
        for(int i=0;i<wlen;i++)
        {
            fr1[radVector[minErrIdx+i].index].score++;
            fr2[fr1[radVector[minErrIdx+i].index].tail].score++;
        }
            


        //计算前后两个图的相对位移，其相对位移等于第二个图的坐标原点在第一个图的坐标

        vector<sortData>dxVector;
        vector<sortData>dyVector;
        for(int i=0;i<fr1.size();i++)
        {
            if(fr1[i].score==2)//triple link and sort by angle
            {
                sortData dx;
                sortData dy;

                double angle=minAngle(mapVector[1].keypoint[fr1[i].tail].angle-mapVector[0].keypoint[i].angle);
                Point p=easyTf(mapVector[0].keypoint[i].pt,mapVector[1].keypoint[fr1[i].tail].pt,deg2rad(angle),lenx,leny);
                dx.data=p.x;
                dx.index=i;
                dy.data=p.y;
                dy.index=i;
                dxVector.push_back(dx);
                dyVector.push_back(dy);

            }
        }
        ROS_INFO("cnt_dxVector=%d",dxVector.size());

        //分别对x和y用滑动窗口滤除
        sort(dxVector.begin(),dxVector.end(),sortFun);
        int xlen=(dxVector.size()*2+1)/3;//加2是为了比如只有1个特征点，长度为1
        int minDxIdx=windowFilter(dxVector,xlen);

        vector<sortData>dyVector2;
        for(int i=0;i<xlen;i++)
        {
            for(int j=0;j<dyVector.size();j++)
            {
                if(dyVector[j].index==dxVector[minDxIdx+i].index)
                    dyVector2.push_back(dyVector[j]);
                
            }
           // ROS_INFO("dx=%lf",dxVector[minDxIdx+i].data);
            //ROS_INFO("dxy=%lf", dyVector2[i].data);
            //ROS_INFO("deg=%f,indx=%d",radVector[minErrIdx+i].angle*180/3.14159,radVector[minErrIdx+i].index);
        }

        sort(dyVector2.begin(),dyVector2.end(),sortFun);
        //for(int i=0;i<dyVector2.size();i++)
            //ROS_INFO("dy=%d",dyVector2[i].index);

        int ylen=(dyVector2.size()*2+1)/3;//加2是为了比如只有1个特征点，长度为1
        int minDyIdx=windowFilter(dyVector2,ylen);
        float rAngle=0;
        

        vector<DMatch>good_matches;
        for(int i=0;i<ylen;i++)
        {
            for(int j=0;j<smatches12.size();j++)
            {
                if(dyVector2[minDyIdx+i].index==smatches12[j].queryIdx)
                {
                    good_matches.push_back(smatches12[j]);
                }
            }
            //ROS_INFO("dy=%lf",dyVector2[minDyIdx+i].data);
        }
        //ROS_INFO("cnt_ylen=%d,cnt_good_matches=%d",ylen,good_matches.size());

        Point drift;
        float gloabAnagle=0;
        Point gloabPoint=Point(0,0);
        for(int i=0;i<ylen;i++)
        {
            float dangle=minAngle(mapVector[1].keypoint[good_matches[i].trainIdx].angle-mapVector[0].keypoint[good_matches[i].queryIdx].angle);
            fr2[good_matches[i].trainIdx].gloabA=fr1[good_matches[i].queryIdx].gloabA+dangle;
            //ROS_INFO("fr1_angle=%.2f",fr1[minDyIdx+i].gloabA);
            //把第二个图的坐标原点在第一个图的坐标转换成世界坐标
            drift=easyTf(mapVector[0].keypoint[good_matches[i].queryIdx].pt,mapVector[1].keypoint[good_matches[i].trainIdx].pt,deg2rad(dangle),lenx,leny);
            
            fr2[good_matches[i].trainIdx].gloabP=easyTf(fr1[good_matches[i].queryIdx].gloabP,drift,deg2rad(fr2[good_matches[i].trainIdx].gloabA),0,0);
            gloabAnagle+=fr2[good_matches[i].trainIdx].gloabA;
            gloabPoint+=fr2[good_matches[i].trainIdx].gloabP;
            //ROS_INFO("relative_angle=%.2f,drift=(%d,%d),gloabp=(%d,%d)",dangle,drift.x,drift.y,fr2[good_matches[i].trainIdx].gloabP.x,fr2[good_matches[i].trainIdx].gloabP.y);
            
            //
            fr2[good_matches[i].trainIdx].score++;
            fr1[good_matches[i].queryIdx].score++;
        
        }
        
        gloabAnagle=gloabAnagle/ylen;        
        gloabPoint = gloabPoint/ylen;

        tm.stop();
        ROS_INFO("all_time=%lf",tm.getTimeSec());

        char text[100];
        sprintf(text,"angle=%.2f,pos=(%d,%d)",gloabAnagle,gloabPoint.x,gloabPoint.y);
        Mat img_goodmatch;
        drawMatches (img1, mapVector[0].keypoint, img2, mapVector[1].keypoint, good_matches, img_goodmatch);
        putText(img_goodmatch, text,Point(600,30), cv::FONT_HERSHEY_TRIPLEX, 0.7, cv::Scalar(0, 255, 0), 1);
        imshow ("orb match", img_goodmatch);

        ROS_INFO("goodAngle=%.2f,goodPos=(%d,%d)",gloabAnagle,gloabPoint.x,gloabPoint.y);
        //map.point=windowFilter(goodPoint);

        //限制一下位移和角度，避免严重错误
        // if((abs(map.angle-mapVector[mapSize-1].angle)>2&&abs(map.angle-mapVector[mapSize-1].angle)<10)
        // ||(abs(map.point.x-mapVector[mapSize-1].point.x)>10&&abs(map.point.x-mapVector[mapSize-1].point.x<100))
        // ||(abs(map.point.y-mapVector[mapSize-1].point.y)>10&&abs(map.point.y-mapVector[mapSize-1].point.y)<100))
        // {
        //     map.keypoint.assign(keypoints2.begin(),keypoints2.end());//复制角点到kt
        //     map.descriptors=descriptors2.clone();//复制描述子
        //     mapVector.push_back(map);
        //     saveData(mapVector[mapSize-1].point,mapVector[mapSize-1].angle);//FPS=18 ,两分钟30kb
        // }

        float lastGangle;
        Point lastGpoint;
        int f1l=fr1.size();
        for(int i=0;i<f1l;i++)
        {
            lastGangle+=fr1[i].gloabA;
            lastGpoint+=fr1[i].gloabP;
        }
        lastGangle/=f1l;
        lastGpoint/=f1l;
        

        for(int i=0;i<fr1.size();i++)
            fr1[i].score=0;
        
        for(int i=0;i<fr2.size();i++)
        {
            fr2[i].score=0;
            fr2[i].head=-1;
        }
            
        
        for(int i=0;i<fr3.size();i++)
            fr3[i].score=0;

        if((abs(lastGangle-gloabAnagle)>2&&(abs(lastGangle-gloabAnagle)<10))||
        (abs(lastGpoint.x-gloabPoint.x)>10&&abs(lastGpoint.x-gloabPoint.x)<100)||
        (abs(lastGpoint.y-gloabPoint.y)>10&&abs(lastGpoint.y-gloabPoint.y)<100))
        {   
            for(int i=0;i<fr2.size();i++)
            {
                if(fr2[i].score<3)
                {
                    fr2[i].gloabP=gloabPoint;
                    fr2[i].gloabA=gloabAnagle;
                }
            }
            fr1.assign(fr2.begin(),fr2.end());
            fr2.assign(fr3.begin(),fr3.end());
            smatches12.assign(smatches23.begin(),smatches23.end());
            mapVector.erase(mapVector.begin());
            saveData(gloabPoint,gloabAnagle);

            cam_pos.p.x=gloabPoint.x;
            cam_pos.p.y=gloabPoint.y;   
            cam_pos.angle=gloabAnagle;
            pub.publish(cam_pos);

        }
        else
        {
            vector<DMatch>matches13;
            vector<DMatch>smatches13;

            matcher.match (mapVector[0].descriptors, mapVector[2].descriptors,matches13);
            selsectMatches(matches13,smatches13);
            smatches12.assign(smatches13.begin(),smatches13.end());

            for(int i=0;i<fr1.size();i++)
                fr1[i].tail=-1;
            
            fr2.assign(fr3.begin(),fr3.end());
            for(int i=0;i<fr2.size();i++)
                fr2[i].head=-1;
                

            for(int i=0;i<smatches12.size();i++)
            {
                fr1[smatches12[i].queryIdx].tail=smatches12[i].trainIdx;
                fr2[smatches12[i].trainIdx].head=smatches12[i].queryIdx;
            }

            mapVector.erase(mapVector.begin()+1);
        }
        waitKey(1);
        
    }

}

//回调函数
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    static int imgCnt=0;
    try
    {
        voSystem(cv_bridge::toCvShare(msg, "bgr8")->image);
            
    }  
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}



int main(int argc, char** argv)
{
    ros::init(argc, argv, "localMap");
    ros::NodeHandle nh;//也有初始化node的作用
    //订阅主题，接收摄像头结点发来的图片
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("camera/image", 1, imageCallback);//camera/image   cv_camera/image_raw

    //创建publisher
    pub = nh.advertise<vslam::position>("position_info", 1);

    //voSystem();
    
    ros::Rate loop_rate(30);
    while (ros::ok())
    {
        ros::spin();
    }
    
    return 0;
}