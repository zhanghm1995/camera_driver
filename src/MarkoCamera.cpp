#include "MarkoCamera.h"
//#include <Windows.h>

//tPvFrame globle_frame_left;
//tPvFrame globle_frame_right;
struct globleframe
{
	tPvFrame globle_frame;
	double data_recv_time;
}globle_frame_left,globle_frame_right;


tCamera GCamera_left_temp;
tCamera GCamera_right_temp;
//
//HANDLE hMutex_left;
//HANDLE hMutex_right;

MakoCamera::~MakoCamera()
{
	StopCamera();
	cvReleaseImage(&left_image);
	cvReleaseImage(&right_image);
	cvReleaseImage(&left_image2);
	cvReleaseImage(&right_image2);
	cvReleaseImage(&tempimgshow);
}
// wait for camera to be plugged in
void MakoCamera::WaitForCamera()
{
	printf("Waiting for a camera\n ");
	double start = clock();
	while((PvCameraCount() == 0) && !GCamera_left.Abort && !GCamera_right.Abort)
	{
		double current = clock();
		printf(".\n");
		sleep(1);
		printf("%f\n",current - start);
		if(current - start > 10)
		{
			return;
		}
	}
	printf("\n");
}

// get the first camera found
// return value: true == success, false == fail
bool MakoCamera::CameraGet()
{
	tPvUint32 count,connected;
	tPvCameraInfoEx list[10];

	//regardless if connected > 1, we only set UID of first camera in list
	printf("CameraGeting..\n");
	count = PvCameraListEx(list,2,&connected, sizeof(tPvCameraInfoEx));
	std::cout<<count<<std::endl;
	if(count == 1)
	{
		GCamera_left.UID = list[0].UniqueId;
		printf("Got only one camera %s\n",list[0].SerialNumber);
	}
	else if (count == 2)
	{
		printf("Got two cameras %s %s\n",list[0].SerialNumber,list[1].SerialNumber);
		GCamera_left.UID = list[0].UniqueId;	
		GCamera_right.UID = list[1].UniqueId;
		return true;
	}
	else
	{
		printf("CameraGet: Failed to find a camera\n");
		return false;
	}
}

// open camera, allocate memory
// return value: true == success, false == fail
bool MakoCamera::LeftCameraSetup()
{
	tPvErr errCode;
	bool failed = false;
	unsigned long FrameSize = 0;

	//open camera
	if ((errCode = PvCameraOpen(GCamera_left.UID,ePvAccessMaster,&(GCamera_left.Handle))) != ePvErrSuccess)
	{
		if (errCode == ePvErrAccessDenied)			
			printf("PvCameraOpen returned ePvErrAccessDenied:\nCamera_left already open as Master, or camera wasn't properly closed and still waiting to HeartbeatTimeout.");
		else
			printf("PvCameraOpen err_left: %u\n", errCode);
		return false;
	}

	// Calculate frame buffer size
	if((errCode = PvAttrUint32Get(GCamera_left.Handle,"TotalBytesPerFrame",&FrameSize)) != ePvErrSuccess)
	{
		printf("CameraSetup: Get TotalBytesPerFrame err_left: %u\n", errCode);
		return false;
	}

	// allocate the frame buffers
	/*GCamera_left.Frame.ImageBuffer = new char[FrameSize];
	if(!GCamera_left.Frame.ImageBuffer)
	{
		printf("CameraSetup: Failed to allocate buffers.\n");
		return false;
	}
	GCamera_left.Frame.ImageBufferSize = FrameSize;*/
	for(int i=0;i<FRAMESCOUNT && !failed;i++)
	{
		GCamera_left.Frame[i].ImageBuffer = new char[FrameSize];
		if(GCamera_left.Frame[i].ImageBuffer)
		{
			GCamera_left.Frame[i].ImageBufferSize = FrameSize;
		}
		else
		{
			printf("CameraSetup: Failed to allocate buffers");
			failed = true;
		}
	}

	return true;
}
bool MakoCamera::RightCameraSetup()
{
	tPvErr errCode;
	bool failed = false;
	unsigned long FrameSize = 0;

	//open camera
	if ((errCode = PvCameraOpen(GCamera_right.UID,ePvAccessMaster,&(GCamera_right.Handle))) != ePvErrSuccess)
	{
		if (errCode == ePvErrAccessDenied)			
			printf("PvCameraOpen returned ePvErrAccessDenied:\nCamera_right already open as Master, or camera wasn't properly closed and still waiting to HeartbeatTimeout.");
		else
			printf("PvCameraOpen err_right: %u\n", errCode);
		return false;
	}

	// Calculate frame buffer size
	if((errCode = PvAttrUint32Get(GCamera_right.Handle,"TotalBytesPerFrame",&FrameSize)) != ePvErrSuccess)
	{
		printf("CameraSetup: Get TotalBytesPerFrame err_right: %u\n", errCode);
		return false;
	}

	// allocate the frame buffers
	/*GCamera_right.Frame.ImageBuffer = new char[FrameSize];
	if(!GCamera_right.Frame.ImageBuffer)
	{
		printf("CameraSetup: Failed to allocate buffers.\n");
		return false;
	}
	GCamera_right.Frame.ImageBufferSize = FrameSize;*/
	for(int i=0;i<FRAMESCOUNT && !failed;i++)
	{
		GCamera_right.Frame[i].ImageBuffer = new char[FrameSize];
		if(GCamera_right.Frame[i].ImageBuffer)
		{
			GCamera_right.Frame[i].ImageBufferSize = FrameSize;
		}
		else
		{
			printf("CameraSetup: Failed to allocate buffers");
			failed = true;
		}
	}
	return true;
}

// close camera, free memory.
void MakoCamera::CameraUnsetup()
{
	tPvErr errCode;
	if(CameraNum==1)
	{
		if((errCode = PvCameraClose(GCamera_left.Handle)) != ePvErrSuccess)
			{
				printf("CameraUnSetup: PvCameraClose err: %u\n", errCode);
			}
			else
			{
				printf("Camera closed.");
			}

		for(int i=0;i<FRAMESCOUNT;i++)
			{
				delete [] (char*)GCamera_left.Frame[i].ImageBuffer;
			}
	}else if(CameraNum==2)
	{

	if((errCode = PvCameraClose(GCamera_left.Handle)) != ePvErrSuccess)
	{
		printf("CameraUnSetup: PvCameraClose err: %u\n", errCode);
	}
	else
	{
		printf("Camera closed.");
	}
	if((errCode = PvCameraClose(GCamera_right.Handle)) != ePvErrSuccess)
	{
		printf("CameraUnSetup: PvCameraClose err: %u\n", errCode);
	}
	else
	{
		printf("Camera closed.");
	}

	// free image buffer
	/*delete [] (char*)GCamera_left.Frame.ImageBuffer;*/
	for(int i=0;i<FRAMESCOUNT;i++)
	{
		delete [] (char*)GCamera_left.Frame[i].ImageBuffer;
		delete [] (char*)GCamera_right.Frame[i].ImageBuffer;
	}
	
}
}

void  MakoCamera::FrameDoneCB_left(tPvFrame* pFrame)
{	
	//Display FrameCount and Status
	printf("%d\n",pFrame->Status);
	if (pFrame->Status == ePvErrSuccess)
	{
		//WaitForSingleObject(hMutex_left,INFINITE);
		printf("into status sucess\n");
		globle_frame_left.globle_frame = *pFrame;
		//globle_frame_left.data_recv_time = playback_globle->SysTime();
		//img_statu_left = true;
		if (globle_frame_left.globle_frame.ImageBuffer==0)
			printf("left image buffer 0\n");
		//ReleaseMutex(hMutex_left);
	}
	else if (pFrame->Status == ePvErrDataMissing)
	{
		//Possible improper network card settings. See GigE Installation Guide.
		printf("Frame_left: %u dropped packets\n", pFrame->FrameCount);
	}
	else if (pFrame->Status == ePvErrCancelled)
	{
		printf("Frame Cancelled\n");
	}
	else
	{
		printf("Frame_left: %u Error: %u\n", pFrame->FrameCount, pFrame->Status);
	}	

	//Requeue frame
	if (pFrame->Status != ePvErrCancelled)
	{
		tPvErr errCode;
		if ((errCode = PvCaptureQueueFrame(GCamera_left_temp.Handle,pFrame,FrameDoneCB_left)) != ePvErrSuccess)
		{
			printf("PvCaptureQueueFrame err: %u           \n", errCode);
		}
	}
}
void  MakoCamera::FrameDoneCB_right(tPvFrame* pFrame)
{	
	//Display FrameCount and Status

	if (pFrame->Status == ePvErrSuccess)
	{
		//WaitForSingleObject(hMutex_right,INFINITE);

		globle_frame_right.globle_frame = *pFrame;
		//globle_frame_right.data_recv_time = playback_globle->SysTime();
		//img_statu_right = true;

		//ReleaseMutex(hMutex_right);
	}
	else if (pFrame->Status == ePvErrDataMissing)
	{
		//Possible improper network card settings. See GigE Installation Guide.
		printf("Frame_right: %u dropped packets\n", pFrame->FrameCount);
	}
	else if (pFrame->Status == ePvErrCancelled)
	{
		printf("Frame Cancelled\n");
	}
	else
	{
		printf("Frame: %u Error: %u\n", pFrame->FrameCount, pFrame->Status);
	}	

	//Requeue frame
	if (pFrame->Status != ePvErrCancelled)
	{
		tPvErr errCode;
		if ((errCode = PvCaptureQueueFrame(GCamera_right_temp.Handle,pFrame,FrameDoneCB_right)) != ePvErrSuccess)
		{
			printf("PvCaptureQueueFrame err: %u           \n", errCode);
		}
	}
}
// setup and start streaming
// return value: true == success, false == fail
bool MakoCamera::LeftCameraStart()
{
	tPvErr errCode;
	bool failed=false;
	// NOTE: This call sets camera PacketSize to largest sized test packet, up to 8228, that doesn't fail
	// on network card. Some MS VISTA network card drivers become unresponsive if test packet fails. 
	// Use PvUint32Set(handle, "PacketSize", MaxAllowablePacketSize) instead. See network card properties
	// for max allowable PacketSize/MTU/JumboFrameSize. 
	sleep(0.1);
	if((errCode = PvCaptureAdjustPacketSize(GCamera_left.Handle,8228)) != ePvErrSuccess)
	{
		printf("CameraStart: PvCaptureAdjustPacketSize err: %u\n", errCode);
		return false;
	}

	// start driver capture stream 
	sleep(0.1);
	if((errCode = PvCaptureStart(GCamera_left.Handle)) != ePvErrSuccess)
	{
		printf("CameraStart: PvCaptureStart err: %u\n", errCode);
		return false;
	}
	GCamera_left_temp = GCamera_left;

	// queue frame
	sleep(0.1);
	//if((errCode = PvCaptureQueueFrame(GCamera_left.Handle,&(GCamera_left.Frame),FrameDoneCB_left)) != ePvErrSuccess)
	//{
	//	printf("CameraStart: PvCaptureQueueFrame err: %u\n", errCode);
	//	// stop driver capture stream
	//	PvCaptureEnd(GCamera_left.Handle);
	//	return false;
	//}
	for(int i=0;i<FRAMESCOUNT && !failed;i++)
	{       
		sleep(0.1);
		if((errCode = PvCaptureQueueFrame(GCamera_left.Handle,&(GCamera_left.Frame[i]),FrameDoneCB_left/*(&(GCamera.Frames[i]),GCamera.Handle,&globle_temp_frame)*/)) != ePvErrSuccess)
		{
			printf("CameraStart: PvCaptureQueueFrame err");
			// stop driver capture stream
			PvCaptureEnd(GCamera_left.Handle);
			failed = true;
		}
	}
	if (failed)
	{
		return false;
	}

	
	if(isHardTrigger)
	{
		if((PvAttrEnumSet(GCamera_left.Handle,"FrameStartTriggerMode","SyncIn2"/*"Freerun"*/) != ePvErrSuccess) ||
			(PvAttrEnumSet(GCamera_left.Handle,"AcquisitionMode","Continuous"/*"SingleFrame"*/) != ePvErrSuccess) ||
			(PvCommandRun(GCamera_left.Handle,"AcquisitionStart") != ePvErrSuccess)||
//			(PvAttrEnumSet(GCamera_left.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
//			(PvAttrEnumSet(GCamera_left.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess)  ||
			(PvAttrEnumSet(GCamera_left.Handle, "ExposureAutoAlg", "Mean") != ePvErrSuccess)||
			(PvAttrUint32Set(GCamera_left.Handle, "ExposureAutoMax", 500000) != ePvErrSuccess)||
			(PvAttrUint32Set(GCamera_left.Handle, "ExposureAutoMin", 12) != ePvErrSuccess))
			{
				printf("CameraStart: failed to set camera attributes\n");
				// clear queued frame
				PvCaptureQueueClear(GCamera_left.Handle);
				// stop driver capture stream
				PvCaptureEnd(GCamera_left.Handle);
				return false;
			}
		if(isAuto)
		{
			if((PvAttrEnumSet(GCamera_left.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
			(PvAttrEnumSet(GCamera_left.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess))
			{
				printf("CameraStart: failed to set camera ExposureMode\n");
				// clear queued frame
				PvCaptureQueueClear(GCamera_left.Handle);
				// stop driver capture stream
				PvCaptureEnd(GCamera_left.Handle);
				return false;
			}
		}
		else
		{
			if((PvAttrEnumSet(GCamera_left.Handle, "ExposureMode","Manual"/*"Auto"*/) != ePvErrSuccess)||
			(PvAttrEnumSet(GCamera_left.Handle, "GainMode", "Manual"/*"Auto"*/) != ePvErrSuccess))
			{
				printf("CameraStart: failed to set camera ExposureMode\n");
				// clear queued frame
				PvCaptureQueueClear(GCamera_left.Handle);
				// stop driver capture stream
				PvCaptureEnd(GCamera_left.Handle);
				return false;
			}
		}
	}
	else
	{
		if((PvAttrEnumSet(GCamera_left.Handle,"FrameStartTriggerMode",/*"SyncIn2"*/"Freerun") != ePvErrSuccess) ||
		(PvAttrEnumSet(GCamera_left.Handle,"AcquisitionMode","Continuous"/*"SingleFrame"*/) != ePvErrSuccess) ||
		(PvCommandRun(GCamera_left.Handle,"AcquisitionStart") != ePvErrSuccess)||
//		(PvAttrEnumSet(GCamera_left.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
//		(PvAttrEnumSet(GCamera_left.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess)  ||
		(PvAttrEnumSet(GCamera_left.Handle, "ExposureAutoAlg", "Mean") != ePvErrSuccess)||
		(PvAttrUint32Set(GCamera_left.Handle, "ExposureAutoMax", 500000) != ePvErrSuccess)||
		(PvAttrUint32Set(GCamera_left.Handle, "ExposureAutoMin", 12) != ePvErrSuccess))
		{
			printf("CameraStart: failed to set camera attributes\n");
			// clear queued frame
			PvCaptureQueueClear(GCamera_left.Handle);
			// stop driver capture stream
			PvCaptureEnd(GCamera_left.Handle);
			return false;
		}
		if(isAuto)
		{
			if((PvAttrEnumSet(GCamera_left.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
			(PvAttrEnumSet(GCamera_left.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess))
			{
				printf("CameraStart: failed to set camera ExposureMode\n");
				// clear queued frame
				PvCaptureQueueClear(GCamera_left.Handle);
				// stop driver capture stream
				PvCaptureEnd(GCamera_left.Handle);
				return false;
			}
		}
		else
		{
			if((PvAttrEnumSet(GCamera_left.Handle, "ExposureMode","Manual"/*"Auto"*/) != ePvErrSuccess)||
			(PvAttrEnumSet(GCamera_left.Handle, "GainMode", "Manual"/*"Auto"*/) != ePvErrSuccess))
			{
				printf("CameraStart: failed to set camera ExposureMode\n");
				// clear queued frame
				PvCaptureQueueClear(GCamera_left.Handle);
				// stop driver capture stream
				PvCaptureEnd(GCamera_left.Handle);
				return false;
			}
		}
	}
	return true;
}
bool MakoCamera::RightCameraStart()
{
	tPvErr errCode;
	bool failed=false;
	// NOTE: This call sets camera PacketSize to largest sized test packet, up to 8228, that doesn't fail
	// on network card. Some MS VISTA network card drivers become unresponsive if test packet fails. 
	// Use PvUint32Set(handle, "PacketSize", MaxAllowablePacketSize) instead. See network card properties
	// for max allowable PacketSize/MTU/JumboFrameSize. 
	sleep(0.1);
	if((errCode = PvCaptureAdjustPacketSize(GCamera_right.Handle,8228)) != ePvErrSuccess)
	{
		printf("CameraStart: PvCaptureAdjustPacketSize err: %u\n", errCode);
		return false;
	}

	// start driver capture stream 
	sleep(0.1);
	if((errCode = PvCaptureStart(GCamera_right.Handle)) != ePvErrSuccess)
	{
		printf("CameraStart: PvCaptureStart err: %u\n", errCode);
		return false;
	}
	GCamera_right_temp = GCamera_right;

	// queue frame
	sleep(0.1);
	//if((errCode = PvCaptureQueueFrame(GCamera_right.Handle,&(GCamera_right.Frame),FrameDoneCB_right)) != ePvErrSuccess)
	//{
	//	printf("CameraStart: PvCaptureQueueFrame err: %u\n", errCode);
	//	// stop driver capture stream
	//	PvCaptureEnd(GCamera_right.Handle);
	//	return false;
	//}
	for(int i=0;i<FRAMESCOUNT && !failed;i++)
	{       
		sleep(0.1);
		if((errCode = PvCaptureQueueFrame(GCamera_right.Handle,&(GCamera_right.Frame[i]),FrameDoneCB_right/*(&(GCamera.Frames[i]),GCamera.Handle,&globle_temp_frame)*/)) != ePvErrSuccess)
		{
			printf("CameraStart: PvCaptureQueueFrame err");
			// stop driver capture stream
			PvCaptureEnd(GCamera_right.Handle);
			failed = true;
		}
	}
	if (failed)
	{
		return false;
	}

	if(isHardTrigger)
	{
		 if((PvAttrEnumSet(GCamera_right.Handle,"FrameStartTriggerMode","SyncIn2"/*"Freerun"*/) != ePvErrSuccess) ||
			(PvAttrEnumSet(GCamera_right.Handle,"AcquisitionMode","Continuous"/*"SingleFrame"*/) != ePvErrSuccess) ||
			(PvCommandRun(GCamera_right.Handle,"AcquisitionStart") != ePvErrSuccess)||
//			(PvAttrEnumSet(GCamera_right.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
//			(PvAttrEnumSet(GCamera_right.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess)  ||
			(PvAttrEnumSet(GCamera_right.Handle, "ExposureAutoAlg", "Mean") != ePvErrSuccess)||
			(PvAttrUint32Set(GCamera_right.Handle, "ExposureAutoMax", 50000) != ePvErrSuccess)||
			(PvAttrUint32Set(GCamera_right.Handle, "ExposureAutoMin", 12) != ePvErrSuccess))
		 	{
				printf("CameraStart: failed to set camera attributes\n");
				// clear queued frame
				PvCaptureQueueClear(GCamera_right.Handle);
				// stop driver capture stream
				PvCaptureEnd(GCamera_right.Handle);
				return false;
			}
		 if(isAuto)
			{
				if((PvAttrEnumSet(GCamera_right.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
				(PvAttrEnumSet(GCamera_right.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess))
				{
					printf("CameraStart: failed to set camera ExposureMode\n");
					// clear queued frame
					PvCaptureQueueClear(GCamera_right.Handle);
					// stop driver capture stream
					PvCaptureEnd(GCamera_right.Handle);
					return false;
				}
			}
			else
			{
				if((PvAttrEnumSet(GCamera_right.Handle, "ExposureMode","Manual"/*"Auto"*/) != ePvErrSuccess)||
				(PvAttrEnumSet(GCamera_right.Handle, "GainMode", "Manual"/*"Auto"*/) != ePvErrSuccess))
				{
					printf("CameraStart: failed to set camera ExposureMode\n");
					// clear queued frame
					PvCaptureQueueClear(GCamera_right.Handle);
					// stop driver capture stream
					PvCaptureEnd(GCamera_right.Handle);
					return false;
				}
			}
	}
	else{
		  if((PvAttrEnumSet(GCamera_right.Handle,"FrameStartTriggerMode",/*"SyncIn2"*/"Freerun") != ePvErrSuccess) ||
			(PvAttrEnumSet(GCamera_right.Handle,"AcquisitionMode","Continuous"/*"SingleFrame"*/) != ePvErrSuccess) ||
			(PvCommandRun(GCamera_right.Handle,"AcquisitionStart") != ePvErrSuccess)||
//			(PvAttrEnumSet(GCamera_right.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
//			(PvAttrEnumSet(GCamera_right.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess)  ||
			(PvAttrEnumSet(GCamera_right.Handle, "ExposureAutoAlg", "Mean") != ePvErrSuccess)||
			(PvAttrUint32Set(GCamera_right.Handle, "ExposureAutoMax", 50000) != ePvErrSuccess)||
			(PvAttrUint32Set(GCamera_right.Handle, "ExposureAutoMin", 12) != ePvErrSuccess))
			{
				printf("CameraStart: failed to set camera attributes\n");
				// clear queued frame
				PvCaptureQueueClear(GCamera_right.Handle);
				// stop driver capture stream
				PvCaptureEnd(GCamera_right.Handle);
				return false;
			}
		  if(isAuto)
			{
				if((PvAttrEnumSet(GCamera_right.Handle, "ExposureMode",/*"Manual"*/"Auto") != ePvErrSuccess)||
				(PvAttrEnumSet(GCamera_right.Handle, "GainMode", /*"Manual"*/"Auto") != ePvErrSuccess))
				{
					printf("CameraStart: failed to set camera ExposureMode\n");
					// clear queued frame
					PvCaptureQueueClear(GCamera_right.Handle);
					// stop driver capture stream
					PvCaptureEnd(GCamera_right.Handle);
					return false;
				}
			}
			else
			{
				if((PvAttrEnumSet(GCamera_right.Handle, "ExposureMode","Manual"/*"Auto"*/) != ePvErrSuccess)||
				(PvAttrEnumSet(GCamera_right.Handle, "GainMode", "Manual"/*"Auto"*/) != ePvErrSuccess))
				{
					printf("CameraStart: failed to set camera ExposureMode\n");
					// clear queued frame
					PvCaptureQueueClear(GCamera_right.Handle);
					// stop driver capture stream
					PvCaptureEnd(GCamera_right.Handle);
					return false;
				}
			}
	     }
	return true;
	
}
// stop streaming
void MakoCamera::CameraStop()
{
	tPvErr errCode;

	if(CameraNum==1)
	{
		//stop camera receiving triggers
			if ((errCode = PvCommandRun(GCamera_left.Handle,"AcquisitionStop")) != ePvErrSuccess &&
					((errCode = PvCommandRun(GCamera_left_temp.Handle,"AcquisitionStop")) != ePvErrSuccess))
				printf("AcquisitionStop command err: %u\n", errCode);
			else
				printf("AcquisitionStop success.\n");

			//PvCaptureQueueClear aborts any actively written frame with Frame.Status = ePvErrDataMissing
			//Further queued frames returned with Frame.Status = ePvErrCancelled

			//Add delay between AcquisitionStop and PvCaptureQueueClear
			//to give actively written frame time to complete
			sleep(0.2);

			printf("\nCalling PvCaptureQueueClear...\n");
			if ((errCode = PvCaptureQueueClear(GCamera_left.Handle)) != ePvErrSuccess &&
					(errCode = PvCaptureQueueClear(GCamera_left_temp.Handle)) != ePvErrSuccess )
				printf("PvCaptureQueueClear err: %u\n", errCode);
			else
				printf("...Queue cleared.\n");

			//stop driver stream
			if ((errCode = PvCaptureEnd(GCamera_left.Handle)) != ePvErrSuccess &&
					(errCode = PvCaptureEnd(GCamera_left.Handle)) != ePvErrSuccess)
				printf("PvCaptureEnd err: %u\n", errCode);
			else
				printf("Driver stream stopped.\n");
	}else if(CameraNum==2)
	{
	//stop camera receiving triggers
	if ((errCode = PvCommandRun(GCamera_left.Handle,"AcquisitionStop")) != ePvErrSuccess &&
			((errCode = PvCommandRun(GCamera_left_temp.Handle,"AcquisitionStop")) != ePvErrSuccess))
		printf("AcquisitionStop command err: %u\n", errCode);
	else
		printf("AcquisitionStop success.\n");

	//PvCaptureQueueClear aborts any actively written frame with Frame.Status = ePvErrDataMissing
	//Further queued frames returned with Frame.Status = ePvErrCancelled

	//Add delay between AcquisitionStop and PvCaptureQueueClear
	//to give actively written frame time to complete
	sleep(0.2);

	printf("\nCalling PvCaptureQueueClear...\n");
	if ((errCode = PvCaptureQueueClear(GCamera_left.Handle)) != ePvErrSuccess &&
			(errCode = PvCaptureQueueClear(GCamera_left_temp.Handle)) != ePvErrSuccess )
		printf("PvCaptureQueueClear err: %u\n", errCode);
	else
		printf("...Queue cleared.\n");  

	//stop driver stream
	if ((errCode = PvCaptureEnd(GCamera_left.Handle)) != ePvErrSuccess &&
			(errCode = PvCaptureEnd(GCamera_left.Handle)) != ePvErrSuccess)
		printf("PvCaptureEnd err: %u\n", errCode);
	else
		printf("Driver stream stopped.\n");


	//right
	//stop camera receiving triggers
	if ((errCode = PvCommandRun(GCamera_right.Handle,"AcquisitionStop")) != ePvErrSuccess &&
			(errCode = PvCommandRun(GCamera_right_temp.Handle,"AcquisitionStop")) != ePvErrSuccess)
		printf("AcquisitionStop command err: %u\n", errCode);
	else
		printf("AcquisitionStop success.\n");

	//PvCaptureQueueClear aborts any actively written frame with Frame.Status = ePvErrDataMissing
	//Further queued frames returned with Frame.Status = ePvErrCancelled

	//Add delay between AcquisitionStop and PvCaptureQueueClear
	//to give actively written frame time to complete
	sleep(0.2);

	printf("\nCalling PvCaptureQueueClear...\n");
	if ((errCode = PvCaptureQueueClear(GCamera_right.Handle)) != ePvErrSuccess &&
			(errCode = PvCaptureQueueClear(GCamera_right_temp.Handle)) != ePvErrSuccess)
		printf("PvCaptureQueueClear err: %u\n", errCode);
	else
		printf("...Queue cleared.\n");  

	//stop driver stream
	if ((errCode = PvCaptureEnd(GCamera_right.Handle)) != ePvErrSuccess &&
			(errCode = PvCaptureEnd(GCamera_right_temp.Handle)) != ePvErrSuccess)
		printf("PvCaptureEnd err: %u\n", errCode);
	else
		printf("Driver stream stopped.\n");
}
}
void MakoCamera::ConvertRawY8RGGB_left(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
	UINT8 *pR,*pB,*pG0,*pG1;
	UINT32 i,j;
	struct pdst
	{
		UINT8 B;
		UINT8 G;
		UINT8 R;
	}*pDst;

	pDst=(struct pdst*)pBGR;

	for(i=0;i<YSize-1;i++)
	{
		if(i&1)
		{
			pG1=pBuf+i*XSize;
			pB=pG1+1;
			pR=pG1+XSize;
			pG0=pR+1;
		}
		else
		{
			pR=pBuf+i*XSize;
			pG0=pR+1;
			pG1=pR+XSize;
			pB=pG1+1;
		}

		// Go through all pixels
		for(j=0;j<XSize-1;j++)
		{
			pDst->B=*pB;
			pDst->G=(UINT8)((*pG0+*pG1)/2);
			pDst->R=*pR;

			pDst++;

			if(j&1)
			{
				pB+=2;
				pG0+=2;
			}
			else
			{
				pR+=2;
				pG1+=2;
			}
		}
		pDst->B=0;
		pDst->G=0;
		pDst->R=0;

		pDst++;
	}

	memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}

void MakoCamera::ConvertRawY8RGGB_right(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
	UINT8 *pR,*pB,*pG0,*pG1;
	UINT32 i,j;
	struct pdst
	{
		UINT8 B;
		UINT8 G;
		UINT8 R;
	}*pDst;

	pDst=(struct pdst*)pBGR;

	for(i=0;i<YSize-1;i++)
	{
		if(i&1)
		{
			pG1=pBuf+i*XSize;
			pB=pG1+1;
			pR=pG1+XSize;
			pG0=pR+1;
		}
		else
		{
			pR=pBuf+i*XSize;
			pG0=pR+1;
			pG1=pR+XSize;
			pB=pG1+1;
		}

		// Go through all pixels
		for(j=0;j<XSize-1;j++)
		{
			pDst->B=*pB;
			pDst->G=(UINT8)((*pG0+*pG1)/2);
			pDst->R=*pR;

			pDst++;

			if(j&1)
			{
				pB+=2;
				pG0+=2;
			}
			else
			{
				pR+=2;
				pG1+=2;
			}
		}
		pDst->B=0;
		pDst->G=0;
		pDst->R=0;

		pDst++;
	}

	memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}


bool MakoCamera::GetImage(void/*tPvHandle *Handle,tPvFrame *pFrame,IplImage *img*/)
{
		//WaitForSingleObject(hMutex_left,INFINITE);
		//WaitForSingleObject(hMutex_right,INFINITE);
	if(CameraNum==1)
	{
		printf("into GetImage/n");
		temp_frame_left = globle_frame_left.globle_frame;
		time_left_recv = globle_frame_left.data_recv_time;
		time_dp = time_left_recv;
		if(temp_frame_left.ImageBuffer !=0)
		{
			ConvertRawY8RGGB_left(temp_frame_left.Width/*IMAGEWIDTH*/,temp_frame_left.Height/*IMAGEHEIGHT*/,(unsigned char *)temp_frame_left.ImageBuffer,(unsigned char*)left_im.data);
			return true;
		}
		printf("after GetImage/n");
	}else if(CameraNum==2)
	{

		temp_frame_left = globle_frame_left.globle_frame;
		temp_frame_right = globle_frame_right.globle_frame;
		time_left_recv = globle_frame_left.data_recv_time;
		time_right_recv = globle_frame_right.data_recv_time;
		time_dp = time_left_recv;
		if(temp_frame_left.ImageBuffer !=0 && temp_frame_right.ImageBuffer != 0 )
		{
			ConvertRawY8RGGB_left(temp_frame_left.Width/*IMAGEWIDTH*/,temp_frame_left.Height/*IMAGEHEIGHT*/,(unsigned char *)temp_frame_left.ImageBuffer,(unsigned char*)left_im.data);
			ConvertRawY8RGGB_right(temp_frame_right.Width/*IMAGEWIDTH*/,temp_frame_right.Height/*IMAGEHEIGHT*/,(unsigned char *)temp_frame_right.ImageBuffer,(unsigned char*)right_im.data);
			return true;
		}
	}
		else
		{
			return false;
		}

		//ReleaseMutex(hMutex_right);
		//ReleaseMutex(hMutex_left);


}
bool MakoCamera::StartCamera()
{
	tPvErr errCode;
	/*char left_camera_filename_save[20];
	char right_camera_filename_save[20];*/



	// initialize the PvAPI
	if((errCode = PvInitialize()) != ePvErrSuccess)
    {
		printf("PvInitialize err: %u\n", errCode);
		return false;
	}
	else
	{
		printf("PvInitialize success!\n");
        //IMPORTANT: Initialize camera structure. See tPvFrame in PvApi.h for more info.
		//memset(&GCamera_left,0,sizeof(tCamera));
		//memset(&GCamera_right,0,sizeof(tCamera));


		//Set function to handle ctrl+C
		//SetConsoleCtrlHandler(CtrlCHandler, true);

		printf("Press CTRL-C to terminate\n");
		   WaitForCamera();
        // wait for a camera to be plugged in
       // WaitForCamera();

        // get first camera found
		if(CameraGet())
		{
			printf("CameraNum:%d/n",CameraNum);
			  if(CameraNum==1)
			  {
			            	 if(LeftCameraSetup())
			            	 {
			            		 printf("start setup\n");
			            	 }
			            	 if(LeftCameraStart())
			            	 {
			            		 printf("start\n");
			            		 sleep(0.1);
			            		 left_file_index=1;

			            		 return true;
			            	 }
			    }else if(CameraNum==2)
			    {
			if(LeftCameraSetup() && RightCameraSetup())
			{
				printf("start setup\n");
				// start camera streaming

				if(LeftCameraStart()&&RightCameraStart())
				{
					printf("start\n");
					sleep(0.1);
					left_file_index = 1;
					right_file_index = 1;

					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		}
		else
			return false;
            // open camera
	}
}
//void MakoCamera::showImage()
//{
//	//GetImage();
////	if(temp_frame_left.ImageBuffer !=0 && temp_frame_right.ImageBuffer != 0 )
////	{
////		ConvertRawY8RGGB_left(1292,964,(unsigned char *)temp_frame_left.ImageBuffer,(unsigned char*)left_image->imageData);
////		ConvertRawY8RGGB_right(1292,964,(unsigned char *)temp_frame_right.ImageBuffer,(unsigned char*)right_image->imageData);
//
//
//		cvResize(left_image,left_image2,CV_INTER_LINEAR);
//		cvResize(right_image,right_image2,CV_INTER_LINEAR);
//
//		cvZero(tempimgshow);
//		cvSetImageROI(tempimgshow,cvRect(0,0,left_image2->width,left_image2->height));
//		cvAdd(left_image2,tempimgshow,tempimgshow,NULL);
//
//		cvSetImageROI(tempimgshow,cvRect(left_image2->width,0,right_image2->width,right_image2->height));
//		cvAdd(right_image2,tempimgshow,tempimgshow,NULL);
//		cvResetImageROI(tempimgshow);
//
//
//		cvShowImage("total",tempimgshow);
//			/*cvShowImage("right",right_image);*/
//		cvWaitKey(1);
////	}
//
//}
void MakoCamera::showImage()
{
//	cvResize(left_image_load,left_image2,CV_INTER_LINEAR);
//	cvResize(right_image_load,right_image2,CV_INTER_LINEAR);


//	cv::resize(left_im,leftim_resize,cv::Size(leftim_resize.cols,rightim_resize.rows));
//	cv::resize(right_im,rightim_resize,cv::Size(leftim_resize.cols,rightim_resize.rows));

	if(CameraNum==1)
	{
		printf("into showImage");
		leftim_resize.copyTo(tempim_show(cv::Rect(0,0,leftim_resize.cols,leftim_resize.rows)));
	}else if(CameraNum==2)
		{
			leftim_resize.copyTo(tempim_show(cv::Rect(0,0,leftim_resize.cols,leftim_resize.rows)));
			rightim_resize.copyTo(tempim_show(cv::Rect(leftim_resize.cols,0,rightim_resize.cols,rightim_resize.rows)));
		}


	cv::imshow("total",tempim_show);
	cvWaitKey(1);

}
void MakoCamera::StopCamera()
{
	  // stop camera streaming
		 CameraStop();
	     // close camera
		 CameraUnsetup();
		  // uninitialize PvAPI
		 PvUnInitialize();
}
void MakoCamera::initParm()
{
	printf("befor globle\n");
	if(CameraNum==1)
	{
		while(globle_frame_left.globle_frame.ImageBuffer == 0)
		{
			printf("into globle\n");
			sleep(1);
		}
		    printf("after globle/n");
			ImageWidth = globle_frame_left.globle_frame.Width;
			ImageHeight = globle_frame_left.globle_frame.Height;

			printf("befor rectify/n");
					if (!rectifyLeft.Init(SettingPath, "LEFT") )
							{
						printf("befor return/n");
						return;
							}
					cv::namedWindow("total",cv::WINDOW_NORMAL);
					left_im = cv::Mat(ImageHeight,ImageWidth,CV_8UC3);
					leftim_resize = cv::Mat(ImageHeight*ScaleFactor,ImageWidth*ScaleFactor,CV_8UC3);
					tempim_show=cv::Mat(ImageHeight*ScaleFactor,ImageWidth*ScaleFactor,CV_8UC3);
					printf("after tempim_show\n");
	}else if(CameraNum==2)
	{
		while(globle_frame_left.globle_frame.ImageBuffer == 0 && globle_frame_right.globle_frame.ImageBuffer == 0 )
		{
			printf("into globle\n");
			sleep(1);
		}
		printf("after globle/n");
		ImageWidth = globle_frame_left.globle_frame.Width;
		ImageHeight = globle_frame_left.globle_frame.Height;

		if (!rectifyLeft.Init(SettingPath, "LEFT") || !rectifyRight.Init(SettingPath, "RIGHT"))
						return ;
cv::namedWindow("total",cv::WINDOW_NORMAL);
left_im = cv::Mat(ImageHeight,ImageWidth,CV_8UC3);
right_im = cv::Mat(ImageHeight,ImageWidth,CV_8UC3);
leftim_resize = cv::Mat(ImageHeight*ScaleFactor,ImageWidth*ScaleFactor,CV_8UC3);
rightim_resize = cv::Mat(ImageHeight*ScaleFactor,ImageWidth*ScaleFactor,CV_8UC3);
tempim_show=cv::Mat(ImageHeight*ScaleFactor,ImageWidth,CV_8UC3);
	}
}
	/*while(globle_frame_left.globle_frame.ImageBuffer == 0 && globle_frame_right.globle_frame.ImageBuffer == 0 )
	{
		printf("into globle\n");
		sleep(1);
	}
	printf("after globle/n");
	ImageWidth = globle_frame_left.globle_frame.Width;
	ImageHeight = globle_frame_left.globle_frame.Height;
*/

//	cvNamedWindow("total",1);
//	left_image = cvCreateImage(cvSize(ImageWidth,ImageHeight),8,3);
//	right_image = cvCreateImage(cvSize(ImageWidth,ImageHeight),8,3);
//	left_image2 = cvCreateImage(cvSize(ImageWidth/2,ImageHeight/2),8,3);
//	right_image2 = cvCreateImage(cvSize(ImageWidth/2,ImageHeight/2),8,3);
//	tempimgshow=cvCreateImage(cvSize(((left_image2->width)+(right_image2->width)),right_image2->height),IPL_DEPTH_8U,3);
/*	if(CameraNum==1)
		{
		printf("befor rectify/n");
		if (!rectifyLeft.Init(SettingPath, "LEFT") )
				{
			printf("befor return/n");
			return;
				}
		cv::namedWindow("total",CV_WINDOW_AUTOSIZE);
		left_im = cv::Mat(ImageHeight,ImageWidth,CV_8UC3);
		leftim_resize = cv::Mat(ImageHeight*ScaleFactor,ImageWidth*ScaleFactor,CV_8UC3);
		tempim_show=cv::Mat(ImageHeight*ScaleFactor,ImageWidth,CV_8UC3);
		printf("after tempim_show\n");
		}*/
void MakoCamera::initParm_playback(string str_left)
{
	cv::Mat im;
	im =cv::imread(str_left,1);
	ImageWidth = im.cols;
	ImageHeight = im.rows;

	if (!rectifyLeft.Init(SettingPath, "LEFT") || !rectifyRight.Init(SettingPath, "RIGHT"))
			return ;

//	cv::Size dSize(int(ImageWidth * 0.5), int(ImageHeight * 0.5));

	cv::namedWindow("total",CV_WINDOW_AUTOSIZE);
	leftim_resize = cv::Mat(ImageHeight*ScaleFactor,ImageWidth*ScaleFactor,CV_8UC3);
	rightim_resize = cv::Mat(ImageHeight*ScaleFactor,ImageWidth*ScaleFactor,CV_8UC3);
	tempim_show=cv::Mat(ImageHeight*ScaleFactor,ImageWidth,CV_8UC3);
}
bool MakoCamera::RectifyImage()
{
	if(CameraNum==1)
		{
		   rectifyLeft.doRectify(left_im, imLeftR);
		   printf("into Tect");
		}else if(CameraNum==2)
		{
	rectifyLeft.doRectify(left_im, imLeftR);
	rectifyRight.doRectify(right_im, imRightR);
		}
}

