#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "Source/Markocamera.h"

cv::Mat image_get(Markocamera markocamera,IplImage test){
    markocamera.CaptureImage();
    cv::Mat test0 = markocamera.src_img;
    test = IplImage(test0);
    IplImage* bgr = cvCreateImage(cvGetSize(&test),8,3);
    if(markocamera.src_img.empty())
        std::cout<<"camera_src.img no odata!!";
    if(test0.empty())
        std::cout<<"test 0: no data in !!";
    cvCvtColor(&test,bgr,CV_BayerBG2BGR);
    cv::Mat image = cv::cvarrToMat(bgr,true);
    cvReleaseImage(&bgr);
    return image;
}
//cv::Mat image_get(Markocamera markocamera,IplImage test){
//    cv::Mat image = cv::imread("/home/cjs/Pictures/1.jpg",1);
//    cv::imshow("imgget",image);
//    cv::waitKey(10);
//    return image;
//}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("image_raw", 1);

  //image capture
  Markocamera m_camera;
  m_camera.Opencamera();
  usleep(1000000);
  ros::Rate loop_rate(30);
  IplImage img_temp;
  while (ros::ok()) {
    cv::Mat image_source = image_get(m_camera,img_temp);
    sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image_source).toImageMsg();
    msg->header.stamp = ros::Time::now();//Important! need add this line for message_filters to sync messages
    pub.publish(msg);
    //ros::spinOnce();
    loop_rate.sleep();
  }
}
