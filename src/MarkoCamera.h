#pragma once

#define _LINUX
#define _x64
//////////////////////////////////////////////////
#include <iostream>
#include <math.h>
using namespace std;

#ifdef _WINDOWS
#include "StdAfx.h"
#endif

#include <stdio.h>
#include <string.h>

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifndef _WINDOWS
#define strncpy_s(dest,len,src,count) strncpy(dest,src,count)
#define sprintf_s(dest,len,format,args...) sprintf(dest,format,args)
#define sscanf_s sscanf
#define strcpy_s(dst,len,src) strcpy(dst,src)
#define _strdup(src) strdup(src)
#define strtok_s(tok,del,ctx) strtok(tok,del)
#endif

#if defined(_LINUX) || defined(_QNX) || defined(_OSX)
#include <unistd.h>
#include <time.h>
#include <signal.h>
#endif
///////////////////////////////////////////////////////
#include "PvApi.h"
#include "ImageLib.h"

#include"ImageRectify.h"

#ifdef _WINDOWS
#define _STDCALL __stdcall
#else
#define _STDCALL
#define TRUE     0
#endif

//#include "cxcore.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/core/core_c.h>
#include<opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <unistd.h>
//#include "iostream"
//#include "time.h"
//#include "math.h"

//#include "stdafx.h"

typedef unsigned int UINT32;
typedef unsigned char  UINT8;

#include "PvApi.h"
#include "ImageLib.h"

//#define SAVE_IMAGE
#define FRAMESCOUNT 15
#define IMAGEWIDTH 1280//1292
#define IMAGEHEIGHT 960//964


typedef struct 
{
	unsigned long   UID;
	tPvHandle       Handle;
	tPvFrame        Frame[15];
	tPvUint32       Counter;
	char            Filename[30];
	bool            Abort;
	
} tCamera;

class MakoCamera
{
public:
	~MakoCamera();
	void WaitForCamera();
	bool CameraGet();
	bool LeftCameraSetup();
	bool RightCameraSetup();
	void CameraUnsetup();
	bool LeftCameraStart();
	bool RightCameraStart();
	void CameraStop();
	
	bool GetImage(void);

	static void FrameDoneCB_left(tPvFrame* pFrame);
	static void FrameDoneCB_right(tPvFrame* pFrame);

	void ConvertRawY8RGGB_left(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);
	void ConvertRawY8RGGB_right(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR);


	tPvFrame temp_frame_left;
	tPvFrame temp_frame_right;

	tCamera GCamera_left;
	tCamera GCamera_right;

	IplImage *left_image;
	IplImage *right_image;
	IplImage *left_image2;
	IplImage *right_image2;
	IplImage *tempimgshow;

	//cv::Mat left_image,right_image,left_image2,right_image2,tempimgshow;

	IplImage *left_image_save;
	IplImage *right_image_save;
	IplImage *left_image_load;
	IplImage *right_image_load;

	cv::Mat left_im,right_im,imLeftR,imRightR;
	cv::Mat leftim_resize,rightim_resize,tempim_show;

	int left_file_index,right_file_index;
	int ImageWidth,ImageHeight;

	char left_camera_filename_save[20];
	char right_camera_filename_save[20];

	//static bool img_statu_left;
    //static bool img_statu_right;
	bool StartCamera();
	//void showImage();
	void StopCamera();
	void showImage();
	void initParm();
	bool RectifyImage();
	void initParm_playback(string str_left);

	ImageRectify rectifyLeft, rectifyRight;
	string SettingPath;


	float ScaleFactor;
	int CameraNum;
	bool isStart;
	bool isShow;
	bool isHardTrigger;
	bool isAuto;
	double time_dp;
private:
	double time_left_recv,time_right_recv;
};
