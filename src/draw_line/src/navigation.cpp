#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <draw_line/position.h>
#include <std_msgs/Float32.h>
#include <cmath>
#include <fstream>
using namespace std;

geometry_msgs::Point cam_pos;
geometry_msgs::Point goalPoint;
visualization_msgs::Marker points, line_strip, line_list,arrow;//line_strip连接所有点,line_list只连接前后两个点
float realAngle;

struct traceData
{
    geometry_msgs::Point point;
    float angle;
};
vector<traceData> traceVector;

int saveData(geometry_msgs::Point &p,float angle)
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

int readData()
{
    traceData t;
    ifstream  inFile;
    inFile.open("/home/lq/Pictures/beautiful.txt");
    if (!inFile.is_open()){
		cout << "open File Failed." << endl;
		return -1;
	}
    string strOne;
	while (getline(inFile, strOne)){
		stringstream ss;
		ss << strOne;
		ss >> t.point.x>> t.point.y>>t.angle;
        t.point.x/=100.0;
        t.point.y/=100.0;
        traceVector.push_back(t);
	}
    
    inFile.close();
}


void markInit(visualization_msgs::Marker &points,visualization_msgs::Marker &line_strip,visualization_msgs::Marker &line_list)
{
    points.header.frame_id = line_strip.header.frame_id = line_list.header.frame_id=arrow.header.frame_id = "center";
    points.header.stamp = line_strip.header.stamp = line_list.header.stamp =arrow.header.stamp= ros::Time::now();
    points.ns = line_strip.ns = line_list.ns =arrow.ns= "points_and_lines";//namespace
    points.action = line_strip.action = line_list.action =arrow.action= visualization_msgs::Marker::ADD;//rviz收到消息后增加点
    points.pose.orientation.w = line_strip.pose.orientation.w = line_list.pose.orientation.w =arrow.pose.orientation.w= 1.0;//四元数w为0，与旋转轴无关，角位移为360的偶数倍
    
    //设置id
    points.id = 0;
    line_strip.id = 1;
    line_list.id = 2;
    arrow.id=3;

    //设置类型
    points.type = visualization_msgs::Marker::POINTS;
    line_strip.type = visualization_msgs::Marker::LINE_STRIP;
    line_list.type = visualization_msgs::Marker::LINE_LIST;
    arrow.type=visualization_msgs::Marker::ARROW;

    // 设置对x,y坐标的缩放比例，值越大点越大
    points.scale.x = 0.1;//对x缩放0.2
    points.scale.y = 0.1;

    line_strip.scale.x = 0.1;
    line_list.scale.x = 0.1;

    arrow.scale.x=0.05;
    arrow.scale.y=0.1;

    //设置颜色，范围从0到1
    points.color.g = 1.0f;
    points.color.a = 1.0;

    line_strip.color.b = 1.0;
    line_strip.color.a = 1.0;

    line_list.color.r = 1.0;
    line_list.color.g = 1.0;
    line_list.color.a = 1.0;

    arrow.color.r=1.0;
    arrow.color.a=1.0;


    geometry_msgs::Point p;
    p.x=0;
    p.y=0;
    arrow.points.push_back(p);
    p.x=0;
    p.y=0;
    arrow.points.push_back(p);
}


vector<double>disVector;
int minIndex=0;
int goalIndx=0;
void analyzeTrace(geometry_msgs::Point &p1,geometry_msgs::Point &p2,float angle)
{
    
    // line_list.points.push_back(p2);
    // p2.z += 0.5;
    // line_list.points.push_back(p2);
    // p2.z=0;

    double minDis=1000;

    for(int i=0;i<traceVector.size();i++)
    {
        double distance=(p1.x-traceVector[i].point.x)*(p1.x-traceVector[i].point.x)+(p1.y-traceVector[i].point.y)*(p1.y-traceVector[i].point.y);
        if(minDis>distance)
        {
            minDis=distance;
            minIndex=i;
        }
        if(p2.x==p1.x&&p2.y==p1.y)
            goalIndx=i;
    }
    arrow.points[0]=p1;
    arrow.points[1]=traceVector[minIndex].point;

    // int forward=1;
    // if(minIndex>goalIndx)forward=-1;
    // for(int i=minIndex;i!=goalIndx-forward;i+=forward)
    // {
    //     arrow.points[0]=traceVector[i].point;
    //     arrow.points[1]=traceVector[i+forward].point;
    // }


}

//画图
void drawTrace(visualization_msgs::Marker &points,visualization_msgs::Marker &line_strip,visualization_msgs::Marker &line_list)
{
    //cam_pos.z=0;
    // points.points.push_back(cam_pos);line_list
    readData();
    for(int i=0;i<traceVector.size();i++)
    {
        line_strip.points.push_back(traceVector[i].point);
        points.points.push_back(traceVector[i].point);
        //line_list.points.push_back(traceVector[i].point);
    }

    
    geometry_msgs::Point p1;
    geometry_msgs::Point p2;
    p1.x=-584/100.0;
    p1.y=264/100.0;

    p2.x=-584/100.0;
    p2.y=218/100.0;

    
    
    //analyzeTrace(p1,p2,0);

     arrow.points[1]=p1;
     arrow.points[0]=p2;
    
//    arrow.points[1].x=1;
//     arrow.points[1].y=1;
//     //放置路标
    // line_list.points.push_back(cam_pos);
    // cam_pos.z += 0.1;
    // line_list.points.push_back(cam_pos);

}


void posCallback(const draw_line::position::ConstPtr &msg)
{
    cam_pos.x=msg->p.x/100;
    cam_pos.y=msg->p.y/100;
    ROS_INFO("angle = %.3f,rvizPos=(%f,%f)",msg->angle,cam_pos.x,cam_pos.y);
    drawTrace(points,line_strip,line_list);//画图
}



int main( int argc, char** argv )
{
    ros::init(argc, argv, "drawLine");
    ros::NodeHandle n;
    ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 10);
    ros::Subscriber sub = n.subscribe("position_info", 1, posCallback);

    //初始化点，线
    markInit(points,line_strip,line_list);
    drawTrace(points,line_strip,line_list);

    ros::Rate r(30);
    int cnt=0;

    int forward=1;
    if(minIndex>goalIndx)forward=-1;
    int idx=minIndex;
    while (ros::ok())
    {
        marker_pub.publish(points);
        marker_pub.publish(line_strip);
        marker_pub.publish(line_list);
        marker_pub.publish(arrow);
        ros::spinOnce();
        // cnt++;
        // if(cnt==30&&idx!=goalIndx-forward)
        // {
        //     cnt=0;
        //     arrow.points[0]=traceVector[idx].point;
        //     arrow.points[1]=traceVector[idx+forward].point;
        //     idx+=forward;
        // }
        r.sleep();
    }
    return 0;
}