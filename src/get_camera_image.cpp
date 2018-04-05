//======================================================================
// Author   : Haiming Zhang
// Email    : zhanghm_1995@qq.com
// Version  :
// Copyright    :
// Descriptoin  : 获取Moko相机图像，并用ROS发布图像消息
// References   :
//======================================================================

#include <time.h>
//ROS
#include <ros/ros.h>
#include <image_transport/image_transport.h> //image handler
#include <cv_bridge/cv_bridge.h>

//project headers
#include "MarkoCamera.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "get_camera_image"); //node name
  ros::NodeHandle nh;
  ros::Publisher pubLeftCameraImg; //发布左相机图像
  pubLeftCameraImg = nh.advertise<sensor_msgs::Image>("left_camera_image",1);
  ros::Rate rate(10); //发布频率

  //set camera parameters and start retriving camera image
  MakoCamera Camera;
  Camera.isShow = true;
  Camera.CameraNum = 1;
  Camera.isHardTrigger = 0;
  Camera.isAuto = 1;
  Camera.ScaleFactor = 0.5; //相机原始像素为1280x960，缩放因子为0.5
  //Camera.SettingPath = "/home/xule/ORB_SLAM2_multilayer/Examples/Stereo/offline_0.5.yaml";//双目标定参数
  Camera.isStart = Camera.StartCamera();


  if(Camera.isStart)
  {
    printf("[INFO] Camera connected!\n");
    Camera.initParm();
  }
  while(ros::ok())
  {
    while(!Camera.GetImage())
    {
      sleep(0.1);
    }
    Camera.RectifyImage();//校正相机图像，校正后图片为imLeftR
    if(Camera.CameraNum==1)
    {
      cv::resize(Camera.imLeftR,Camera.leftim_resize,cv::Size(Camera.leftim_resize.cols,Camera.leftim_resize.rows));
    }else if(Camera.CameraNum==2)
    {
      cv::resize(Camera.imLeftR,Camera.leftim_resize,cv::Size(Camera.leftim_resize.cols,Camera.rightim_resize.rows));
      cv::resize(Camera.imRightR,Camera.rightim_resize,cv::Size(Camera.leftim_resize.cols,Camera.rightim_resize.rows));
    }

    //publish image to ROS node
    sensor_msgs::ImagePtr left_image_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", Camera.leftim_resize).toImageMsg();
    left_image_msg.header.stamp = ros::Time::now();
    pubLeftCameraImg.publish(left_image_msg);
    if(Camera.isShow)
    {
      Camera.showImage();
    }

    rate.sleep();
  }
}

