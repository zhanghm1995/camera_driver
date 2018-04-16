#include "Markocamera.h"

void Markocamera::Opencamera(){
	err = VmbErrorSuccess;
	AVT::VmbAPI::Examples::ProgramConfig Config;
	err = Config.ParseCommandline(0,0);

        //Write out an error if we could not parse the command line
    if ( VmbErrorBadParameter == err )
    {
        std::cout<< "Invalid parameters!\n\n" ;
        Config.setPrintHelp( true );
    }

    //Print out help and end program
    if ( Config.getPrintHelp() )
    {
        Config.PrintHelp( std::cout );
    }
    else
    {
    	AVT::VmbAPI::Examples::ApiController apiController;
    	// Print out version of Vimba
        std::cout<<"Vimba C++ API Version "<<apiController.GetVersion()<<"\n";
        // Startup Vimba
        err = apiController.StartUp();        
        if ( VmbErrorSuccess == err )
        {
            if( Config.getCameraID().empty() )
            {
                AVT::VmbAPI::CameraPtrVector cameras = apiController.GetCameraList();
                if( cameras.empty() )
                {
                    err = VmbErrorNotFound;
                }
                else
                {
                    std::string strCameraID;
                    err = cameras[0]->GetID( strCameraID );
                    if( VmbErrorSuccess == err )
                    {
                        Config.setCameraID( strCameraID );
                    }
                }
            }
            if ( VmbErrorSuccess == err )
            {
                std::cout<<"Opening camera with ID: "<<Config.getCameraID()<<"\n";
            
            err = apiController.StartContinuousImageAcquisition( Config );
            if ( VmbErrorSuccess == err )
                {                
                    std::cout<< "Successfully load the camera\n" ;
                }
            }
      }       
}
}

bool Markocamera::CaptureImage(){
	if(!(AVT::VmbAPI::Examples::FrameObserver::src.empty())){
		src_img = AVT::VmbAPI::Examples::FrameObserver::src;
		if(!src_img.empty()){
			std::cout<<"image received!\n";
			return true;
			}
		else {
			std::cout<<"there is no data in the image !\n";
			return false;
		}
	}
	else {
		std::cout<<"didn't received any data in the capturing step !\n";
		return false; 	
	}
}



void Markocamera::Closecamera(){
	AVT::VmbAPI::Examples::ApiController apiController;
	apiController.StopContinuousImageAcquisition();
	std::cout<<"Acquisition stopped.\n";
	apiController.ShutDown();
}

