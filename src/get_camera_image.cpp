/*
 * get_camera_image.cpp
 *
 *  Created on: 2018年3月27日
 *      Author: zhanghm
 */

#include <time.h>
//ROS
#include <ros/ros.h>
//project headers
#include "MarkoCamera.h"

int main()
{
    MakoCamera Camera;
    Camera.isShow = true;
    Camera.CameraNum = 1;
    Camera.isHardTrigger = 0;
    Camera.isAuto = 1;
    Camera.ScaleFactor = 0.5;
    Camera.SettingPath = "/home/xule/ORB_SLAM2_multilayer/Examples/Stereo/offline_0.5.yaml";
    Camera.isStart = Camera.StartCamera();


    if(Camera.isStart)
    {
        Camera.initParm();
    }
    int i = 1;
    while(i)
    {
        while(!Camera.GetImage())
        {
            sleep(0.1);
        }
        Camera.RectifyImage();
        if(Camera.CameraNum==1)
        {
            cv::resize(Camera.imLeftR,Camera.leftim_resize,cv::Size(Camera.leftim_resize.cols,Camera.leftim_resize.rows));
        }else if(Camera.CameraNum==2)
        {
            cv::resize(Camera.imLeftR,Camera.leftim_resize,cv::Size(Camera.leftim_resize.cols,Camera.rightim_resize.rows));
            cv::resize(Camera.imRightR,Camera.rightim_resize,cv::Size(Camera.leftim_resize.cols,Camera.rightim_resize.rows));
        }

        if(Camera.isShow)
        {
            Camera.showImage();
        }
    }
}

