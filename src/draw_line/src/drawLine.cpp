#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <draw_line/position.h>
#include <std_msgs/Float32.h>
#include <cmath>

geometry_msgs::Point cam_pos;
visualization_msgs::Marker points, line_strip, line_list;//line_strip连接所有点,line_list只连接前后两个点

void markInit(visualization_msgs::Marker &points,visualization_msgs::Marker &line_strip,visualization_msgs::Marker &line_list)
{
    points.header.frame_id = line_strip.header.frame_id = line_list.header.frame_id = "center";
    points.header.stamp = line_strip.header.stamp = line_list.header.stamp = ros::Time::now();
    points.ns = line_strip.ns = line_list.ns = "points_and_lines";//namespace
    points.action = line_strip.action = line_list.action = visualization_msgs::Marker::ADD;//rviz收到消息后增加点
    points.pose.orientation.w = line_strip.pose.orientation.w = line_list.pose.orientation.w = 1.0;//四元数w为0，与旋转轴无关，角位移为360的偶数倍
    
    //设置id
    points.id = 0;
    line_strip.id = 1;
    line_list.id = 2;

    //设置类型
    points.type = visualization_msgs::Marker::POINTS;
    line_strip.type = visualization_msgs::Marker::LINE_STRIP;
    line_list.type = visualization_msgs::Marker::LINE_LIST;


    // 设置对x,y坐标的缩放比例，值越大点越大
    points.scale.x = 0.4;//对x缩放0.2
    points.scale.y = 0.4;

    line_strip.scale.x = 0.1;
    line_list.scale.x = 0.1;

    //设置颜色，范围从0到1
    points.color.g = 1.0f;
    points.color.a = 1.0;

    line_strip.color.b = 1.0;
    line_strip.color.a = 1.0;

    line_list.color.r = 1.0;
    line_list.color.a = 1.0;
}



//画图
void analyzeTrace(visualization_msgs::Marker &points,visualization_msgs::Marker &line_strip,visualization_msgs::Marker &line_list)
{

    // cam_pos.z=0;
    points.points.push_back(cam_pos);
    line_strip.points.push_back(cam_pos);


    //放置路标
    // line_list.points.push_back(cam_pos);
    // cam_pos.z += 0.1;
    // line_list.points.push_back(cam_pos);

}


void posCallback(const draw_line::position::ConstPtr &msg)
{
    cam_pos.x=msg->p.x/100;
    cam_pos.y=msg->p.y/100;
    ROS_INFO("angle = %.3f,rvizPos=(%f,%f)",msg->angle,cam_pos.x,cam_pos.y);
    analyzeTrace(points,line_strip,line_list);//画图
}



int main( int argc, char** argv )
{
    ros::init(argc, argv, "drawLine");
    ros::NodeHandle n;
    ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 10);
    ros::Subscriber sub = n.subscribe("position_info", 1, posCallback);

    //初始化点，线
    markInit(points,line_strip,line_list);

    ros::Rate r(30);
    while (ros::ok())
    {
        marker_pub.publish(points);
        marker_pub.publish(line_strip);
        marker_pub.publish(line_list);
        ros::spinOnce();
        r.sleep();
    }
    return 0;
}