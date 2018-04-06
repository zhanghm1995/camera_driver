#include "PG_camera.h"

bool PGcamera::StartCamera(){
    BusManager busMgr;
    Error error;

    error = busMgr.GetCameraFromIndex(0,&guid);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return false;
    }

    //Connect to a camera
    error = camera.Connect(&guid);
    if(error!=PGRERROR_OK)
    {
        cout<<"Can not connect to Camera"<<endl;
        return false;
    }


    FC2Config config;//?????
    error = camera.GetConfiguration(&config);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return false;
    }

    // Set the camera configuration
    error = camera.SetConfiguration( &config );
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return false;
    }
    //Start capturing images
    error = camera.StartCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );

        return false;
    }
    else {
        cout<<"starting capturing image"<<endl;
    }
    return true;
}
void PGcamera::PrintError( Error error )
{
    error.PrintErrorTrace();
}

IplImage* PGcamera::CaptureImage(){
    Error error;
    Image rawImage;
    // Retrieve an image
    error = camera.RetrieveBuffer( &rawImage );
    if (error != PGRERROR_OK)
    {
        PrintError( error );

    }

 //   cout << "Grabbed image " << endl;
    // Create a converted image


    Image convertedImage;

    // Convert the raw image
    error = rawImage.Convert( PIXEL_FORMAT_MONO8, &convertedImage );
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        //return ;
    }

    //Image convertedImage;
    //convertedImage.DeepCopy(&rawImage);

    //convert to opencv image
    int width = convertedImage.GetCols();
    int height = convertedImage.GetRows();

    IplImage * frame;
    frame = cvCreateImage(cvSize(width,height),8,1);
    const unsigned char* pImageBuffer = convertedImage.GetData();
    for (int i = 0;i < height;i++)
    {
            for (int j = 0;j < width;j++)
            {
                *(frame->imageData + i*frame->widthStep + j) = pImageBuffer[i*width + j];
            }
    }
    IplImage * frame1 = cvCreateImage(cvSize(width,height),8,1);
    cvCopy(frame,frame1);
    //cvCvtColor(frame,frame1,CV_BGR2GRAY);

return frame1;
 //   cvShowImage("111",frame1);
  //  cvWaitKey(0);

   /* cv::Mat fra = cv::Mat(width,height,CV_8UC3,convertedImage);
    *frame1 = IplImage(fra);*/

}

void PGcamera::CloseCamera(){
    Error error;

    //Stop capturing images
    error = camera.StopCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return;
    }

    //Disconnect to camera
    error = camera.Disconnect();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return;
    }
}

