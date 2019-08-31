# visual-odometry

- 基于天开花板视觉的视觉里程计，实现使用opencv调用usb摄像头、ORB处理特征点，滤除错误匹配并计算全局坐标，使用rviz绘制移动轨迹。

- 以前的项目，工程节构有点小乱，工程中有相关的测试节点，懒得整理，上传以备用。

- ros版本：kinect。要切换到melodic请选择melodic分支。两个版本的不同主要在于opencv路径

- 插上usb摄像头后。运行rviz，然后运行以下代码

  ```roslaunch vslam launchTest.launch```
- 如出错需要安装find_object_2d信赖。[参考网址](http://wiki.ros.org/find_object_2d)
