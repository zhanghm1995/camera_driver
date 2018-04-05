//======================================================================
// Author   : Haiming Zhang
// Email    : zhanghm_1995@qq.com
// Version  :
// Copyright    :
// Descriptoin  : PG相机图像获取类
// References   :
//======================================================================
#ifndef SRC_CAMERA_DRIVER_SRC_PG_CAMERA_H_
#define SRC_CAMERA_DRIVER_SRC_PG_CAMERA_H_
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
//OpenCV
#include <opencv2/opencv.hpp>
#include "FlyCapture2.h"
using namespace FlyCapture2;
using namespace std;

class PGcamera
{
public:
    PGcamera() {}
    ~PGcamera(){}
    bool StartCamera();
    void CloseCamera();
    IplImage* CaptureImage();
    void PrintError( Error error );


    Camera camera;
    //IplImage* img_src;

private:
    PGRGuid guid;
};/*SRC_CAMERA_DRIVER_SRC_PG_CAMERA_H_*/
