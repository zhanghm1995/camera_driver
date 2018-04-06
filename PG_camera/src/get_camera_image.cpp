//======================================================================
// Author   : Haiming Zhang
// Email    : zhanghm_1995@qq.com
// Version  :
// Copyright    :
// Descriptoin  : 获取PG相机图像，并用ROS发布图像消息
// References   :
//======================================================================

#include <time.h>
//ROS
#include <ros/ros.h>
#include <image_transport/image_transport.h> //image handler
#include <cv_bridge/cv_bridge.h>

//project headers
#include "PG_camera.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "get_camera_image"); //node name
  ros::NodeHandle nh;
  ros::Publisher pubLeftCameraImg; //发布左相机图像
  pubLeftCameraImg = nh.advertise<sensor_msgs::Image>("left_camera_image",1);
  ros::Rate rate(10); //发布频率

  //set camera parameters and start retriving camera image
  PGcamera Camera;
  bool camera_opened = Camera.StartCamera();


  if(!camera_opened)
  {
    printf("[INFO] Camera cannot connected!!\n");
    return -1;
  }
  while(ros::ok())
  {
    IplImage* image_src = Camera.CaptureImage();//获取图片,image_src指针指向图像内存区域，需要负责释放
    cv::Mat mat_img = cv::cvarrToMat(image_src,false);//不复制图像转换为Mat类型图片
//    cv::cvtColor(mat_img,mat_img,CV_GRAY2BGR);

    //publish image to ROS node
    sensor_msgs::ImagePtr left_image_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", mat_img).toImageMsg();
    left_image_msg->header.stamp = ros::Time::now();
    pubLeftCameraImg.publish(left_image_msg);
    cv::imshow("PG camera",mat_img);
    cv::waitKey(30);
    cvReleaseImage(&image_src);
    rate.sleep();
  }
}

