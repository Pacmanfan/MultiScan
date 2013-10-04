// cvStructuredLighting.cpp: Defines the entry point for the console application.
//
// Overview:
//   This file defines the main console application function for structured light scanning.
//   System parameters and scanning options are provided in an XML-formatted file. Output 
//   images, point clouds, and calibration parameters are stored in a user-defined directory.
//
// Details:
//   Please read the SIGGRAPH 2009 course notes for additional details.
//
//     Douglas Lanman and Gabriel Taubin
//     "Build Your Own 3D Scanner: 3D Photography for Beginners"
//     ACM SIGGRAPH 2009 Course Notes
//
// Author:
//   Douglas Lanman
//   Brown University
//   July 2009

#include "stdafx.h"
#include <direct.h>
#include <conio.h>
#include "cvStructuredLight.h"
#include "cvCalibrateProCam.h"
#include "cvScanProCam.h"
#include "cvUtilProCam.h"

struct slParams sl_params; // the parameters to use for scanning
struct slCalib sl_calib;


void CreateDefaultProjectorCamera()
{
	// Allocate storage for calibration parameters.	
	int cam_nelems                  = sl_params.cam_w*sl_params.cam_h;
	int proj_nelems                 = sl_params.proj_w*sl_params.proj_h;
    sl_calib.cam_intrinsic_calib    = false;
	sl_calib.proj_intrinsic_calib   = false;
	sl_calib.procam_extrinsic_calib = false;
	sl_calib.cam_intrinsic          = cvCreateMat(3,3,CV_32FC1);
	sl_calib.cam_distortion         = cvCreateMat(5,1,CV_32FC1);
	sl_calib.cam_extrinsic          = cvCreateMat(2, 3, CV_32FC1);
	sl_calib.proj_intrinsic         = cvCreateMat(3, 3, CV_32FC1);
	sl_calib.proj_distortion        = cvCreateMat(5, 1, CV_32FC1);
	sl_calib.proj_extrinsic         = cvCreateMat(2, 3, CV_32FC1);
	sl_calib.cam_center             = cvCreateMat(3, 1, CV_32FC1);
	sl_calib.proj_center            = cvCreateMat(3, 1, CV_32FC1);
	sl_calib.cam_rays               = cvCreateMat(3, cam_nelems, CV_32FC1);
	sl_calib.proj_rays              = cvCreateMat(3, proj_nelems, CV_32FC1);
	sl_calib.proj_column_planes     = cvCreateMat(sl_params.proj_w, 4, CV_32FC1);
	sl_calib.proj_row_planes        = cvCreateMat(sl_params.proj_h, 4, CV_32FC1);
}

int LoadCameraProjector()
{
	// Load intrinsic camera calibration parameters (if found).
	char str1[1024], str2[1024];
	sprintf(str1, "%s\\calib\\cam\\cam_intrinsic.xml",  sl_params.outdir);
	sprintf(str2, "%s\\calib\\cam\\cam_distortion.xml", sl_params.outdir);
	if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
		sl_calib.cam_intrinsic  = (CvMat*)cvLoad(str1);
		sl_calib.cam_distortion = (CvMat*)cvLoad(str2);
		sl_calib.cam_intrinsic_calib = true;
		printf("Loaded previous intrinsic camera calibration.\n");
	}
	else
		printf("Camera has not been intrinsically calibrated!\n");

	// Load intrinsic projector calibration parameters (if found);
	sprintf(str1, "%s\\calib\\proj\\proj_intrinsic.xml",  sl_params.outdir);
	sprintf(str2, "%s\\calib\\proj\\proj_distortion.xml", sl_params.outdir);
	if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
		sl_calib.proj_intrinsic  = (CvMat*)cvLoad(str1);
		sl_calib.proj_distortion = (CvMat*)cvLoad(str2);
		sl_calib.proj_intrinsic_calib = true;
		printf("Loaded previous intrinsic projector calibration.\n");
	}
	else
		printf("Projector has not been intrinsically calibrated!\n");

	// Load extrinsic projector-camera parameters (if found).
	sprintf(str1, "%s\\calib\\proj\\cam_extrinsic.xml",  sl_params.outdir);
	sprintf(str2, "%s\\calib\\proj\\proj_extrinsic.xml", sl_params.outdir);
	if( (sl_calib.cam_intrinsic_calib && sl_calib.proj_intrinsic_calib) &&
		( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ) ){
		sl_calib.cam_extrinsic  = (CvMat*)cvLoad(str1);
		sl_calib.proj_extrinsic = (CvMat*)cvLoad(str2);
		sl_calib.procam_extrinsic_calib = true;
		evaluateProCamGeometry(&sl_params, &sl_calib);
		printf("Loaded previous extrinsic projector-camera calibration.\n");
	}
	else
		printf("Projector-camera system has not been extrinsically calibrated!\n");

	// Initialize background model.
	sl_calib.background_depth_map = cvCreateMat(sl_params.cam_h, sl_params.cam_w, CV_32FC1);
	sl_calib.background_image     = cvCreateImage(cvSize(sl_params.cam_w, sl_params.cam_h), IPL_DEPTH_8U, 3);
	sl_calib.background_mask      = cvCreateImage(cvSize(sl_params.cam_w, sl_params.cam_h), IPL_DEPTH_8U, 1);
	cvSet(sl_calib.background_depth_map, cvScalar(FLT_MAX));
	cvZero(sl_calib.background_image);
	cvSet(sl_calib.background_mask, cvScalar(255));

	return 1;
}

/*
Since this was all taken from a command-line app, we need to break 
apart the main function in to a few more functions
- Load the config and initialize a few things
*/
int LoadConfig()
{
	char configFile[1024];
	strcpy(configFile, "./config.xml");

	FILE* pFile = fopen(configFile, "r");
	if(pFile != NULL)
	{
		fclose(pFile);
		readConfiguration(configFile, &sl_params);
	}
	else
	{
		return 0;
	}
}

int SaveConfig()
{
	char configFile[1024];
	strcpy(configFile, "./config.xml");
	writeConfiguration(configFile, &sl_params);
	return 1;
}
// Define main routine (i.e., entry point for the console application).
int main(int argc, char* argv[])
{
	// Parse command line arguments.
	printf("[Structured Lighting for 3D Scanning]\n");
	char configFile[1024];
	if(argc == 1)
		strcpy(configFile, "./config.xml");
	else
		strcpy(configFile, argv[1]);

	// Read structured lighting parameters from configuration file.
	struct slParams sl_params;
	FILE* pFile = fopen(configFile, "r");
	if(pFile != NULL){
		fclose(pFile);
		printf("Reading configuration file \"%s\"...\n", configFile);
		readConfiguration(configFile, &sl_params);
	}
	else{
		printf("ERROR: Could not open configuration file \"%s\"!\n", configFile);
		printf("Press any key to exit.\n");
		_getch();
		return -1;
	}

	// Initialize capture from any detected device.
	printf("Initializing camera and projector...\n"); 
	if(sl_params.Logitech_9000){
		printf("Enabling Bayer mode for Logitech QuickCam 9000...\n");
		system("Bayer.exe 1 10 > nul");
	}
	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
	if(capture == NULL){
		printf("ERROR: No camera was detected by OpenCV!\n");
		printf("Press any key to exit.\n");
		_getch();
		return -1;
	}
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,  sl_params.cam_w);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, sl_params.cam_h);
	IplImage* cam_frame = cvQueryFrame2(capture, &sl_params);
	if(cam_frame == NULL){
		printf("ERROR: No frame was available!\n");
		printf("Press any key to exit.\n");
		_getch();
		return -1;
	}
	
	// Create fullscreen window (for controlling projector display).
	cvNamedWindow("projWindow", CV_WINDOW_AUTOSIZE);
	IplImage* proj_frame = cvCreateImage(cvSize(sl_params.proj_w, sl_params.proj_h), IPL_DEPTH_8U, 3);
	cvSet(proj_frame, cvScalar(0, 0, 0));
	cvShowImage("projWindow", proj_frame);
	cvMoveWindow("projWindow", -sl_params.proj_w-7, -33);
	cvWaitKey(1);
	
	// Create output directory (clear previous scan first).
	printf("Creating output directory (overwrites existing object data)...\n");
	char str[1024];
	_mkdir(sl_params.outdir);
	sprintf(str, "%s\\%s", sl_params.outdir, sl_params.object);
	_mkdir(str);
	sprintf(str, "rd /s /q \"%s\\%s\"", sl_params.outdir, sl_params.object);
	system(str);
	sprintf(str, "%s\\%s", sl_params.outdir, sl_params.object);
	if(_mkdir(str) != 0){
		printf("ERROR: Cannot open output directory!\n");
		printf("Press any key to exit.\n");
		_getch();
		return -1;
	}
	
	// Allocate storage for calibration parameters.
	struct slCalib sl_calib;
	int cam_nelems                  = sl_params.cam_w*sl_params.cam_h;
	int proj_nelems                 = sl_params.proj_w*sl_params.proj_h;
    sl_calib.cam_intrinsic_calib    = false;
	sl_calib.proj_intrinsic_calib   = false;
	sl_calib.procam_extrinsic_calib = false;
	sl_calib.cam_intrinsic          = cvCreateMat(3,3,CV_32FC1);
	sl_calib.cam_distortion         = cvCreateMat(5,1,CV_32FC1);
	sl_calib.cam_extrinsic          = cvCreateMat(2, 3, CV_32FC1);
	sl_calib.proj_intrinsic         = cvCreateMat(3, 3, CV_32FC1);
	sl_calib.proj_distortion        = cvCreateMat(5, 1, CV_32FC1);
	sl_calib.proj_extrinsic         = cvCreateMat(2, 3, CV_32FC1);
	sl_calib.cam_center             = cvCreateMat(3, 1, CV_32FC1);
	sl_calib.proj_center            = cvCreateMat(3, 1, CV_32FC1);
	sl_calib.cam_rays               = cvCreateMat(3, cam_nelems, CV_32FC1);
	sl_calib.proj_rays              = cvCreateMat(3, proj_nelems, CV_32FC1);
	sl_calib.proj_column_planes     = cvCreateMat(sl_params.proj_w, 4, CV_32FC1);
	sl_calib.proj_row_planes        = cvCreateMat(sl_params.proj_h, 4, CV_32FC1);
	
	// Load intrinsic camera calibration parameters (if found).
	char str1[1024], str2[1024];
	sprintf(str1, "%s\\calib\\cam\\cam_intrinsic.xml",  sl_params.outdir);
	sprintf(str2, "%s\\calib\\cam\\cam_distortion.xml", sl_params.outdir);
	if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
		sl_calib.cam_intrinsic  = (CvMat*)cvLoad(str1);
		sl_calib.cam_distortion = (CvMat*)cvLoad(str2);
		sl_calib.cam_intrinsic_calib = true;
		printf("Loaded previous intrinsic camera calibration.\n");
	}
	else
		printf("Camera has not been intrinsically calibrated!\n");

	// Load intrinsic projector calibration parameters (if found);
	sprintf(str1, "%s\\calib\\proj\\proj_intrinsic.xml",  sl_params.outdir);
	sprintf(str2, "%s\\calib\\proj\\proj_distortion.xml", sl_params.outdir);
	if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
		sl_calib.proj_intrinsic  = (CvMat*)cvLoad(str1);
		sl_calib.proj_distortion = (CvMat*)cvLoad(str2);
		sl_calib.proj_intrinsic_calib = true;
		printf("Loaded previous intrinsic projector calibration.\n");
	}
	else
		printf("Projector has not been intrinsically calibrated!\n");

	// Load extrinsic projector-camera parameters (if found).
	sprintf(str1, "%s\\calib\\proj\\cam_extrinsic.xml",  sl_params.outdir);
	sprintf(str2, "%s\\calib\\proj\\proj_extrinsic.xml", sl_params.outdir);
	if( (sl_calib.cam_intrinsic_calib && sl_calib.proj_intrinsic_calib) &&
		( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ) ){
		sl_calib.cam_extrinsic  = (CvMat*)cvLoad(str1);
		sl_calib.proj_extrinsic = (CvMat*)cvLoad(str2);
		sl_calib.procam_extrinsic_calib = true;
		evaluateProCamGeometry(&sl_params, &sl_calib);
		printf("Loaded previous extrinsic projector-camera calibration.\n");
	}
	else
		printf("Projector-camera system has not been extrinsically calibrated!\n");

	// Initialize background model.
	sl_calib.background_depth_map = cvCreateMat(sl_params.cam_h, sl_params.cam_w, CV_32FC1);
	sl_calib.background_image     = cvCreateImage(cvSize(sl_params.cam_w, sl_params.cam_h), IPL_DEPTH_8U, 3);
	sl_calib.background_mask      = cvCreateImage(cvSize(sl_params.cam_w, sl_params.cam_h), IPL_DEPTH_8U, 1);
	cvSet(sl_calib.background_depth_map, cvScalar(FLT_MAX));
	cvZero(sl_calib.background_image);
	cvSet(sl_calib.background_mask, cvScalar(255));

	// Initialize scan counter (used to index each scan iteration).
	int scan_index = 0;

	// Process user input, until 'ESC' is pressed.
	int cvKey = NULL;
	while(1){

		// Display a black projector image by default.
		cvSet(proj_frame, cvScalar(0, 0, 0));
		cvShowImage("projWindow", proj_frame);
		cvWaitKey(1);

		// Parse keystroke.
		if(cvKey == 27){
			printf("\n> Writing configuration file \"%s\"...\n", configFile);
			writeConfiguration(configFile, &sl_params);
			if(sl_params.Logitech_9000){
				printf("> Disabling Bayer mode for Logitech QuickCam 9000...\n");
				system("Bayer.exe 0 > nul");
			}
			printf("> Exiting application...\n");
			break;
		}
		else if(cvKey == 's'){
			printf("\n> Running scanner (view %d)...\n", ++scan_index);
			runStructuredLight(capture, &sl_params, &sl_calib, scan_index);
			cvKey = NULL;
		}
		else if(cvKey == 'b'){
			printf("\n> Scanning background...\n");
			cvSet(sl_calib.background_depth_map, cvScalar(FLT_MAX));
			cvZero(sl_calib.background_image);
			cvSet(sl_calib.background_mask, cvScalar(255));
			runBackgroundCapture(capture, &sl_params, &sl_calib);
			cvKey = NULL;
		}
		else if(cvKey == 'r'){
			printf("\n> Resetting background...\n");
			cvSet(sl_calib.background_depth_map, cvScalar(FLT_MAX));
			cvZero(sl_calib.background_image);
			cvSet(sl_calib.background_mask, cvScalar(255));
			cvKey = NULL;
		}
		else if(cvKey == 'c'){
			printf("\n> Calibrating camera...\n");
			runCameraCalibration(capture, &sl_params, &sl_calib);
			cvKey = NULL;
		}
		else if(cvKey == 'p'){
			printf("\n> Calibrating projector...\n");
			runProjectorCalibration(capture, &sl_params, &sl_calib, false);
			cvKey = NULL;
		}
		else if(cvKey == 'a'){
			printf("\n> Calibrating camera and projector simultaneously...\n");
			runProjectorCalibration(capture, &sl_params, &sl_calib, true);
			cvKey = NULL;
		}
		else if(cvKey == 'e'){
			printf("\n> Calibrating projector-camera alignment...\n");
			runProCamExtrinsicCalibration(capture, &sl_params, &sl_calib);
			cvKey = NULL;
		}

		// Display prompt.
		if(cvKey == NULL){
			printf("\nPress the following keys for the corresponding functions.\n");
			printf("'S': Run scanner\n");
			printf("'B': Estimate background\n");
			printf("'R': Reset background\n");
			printf("'C': Calibrate camera\n");
			printf("'P': Calibrate projector\n");
			printf("'A': Calibrate camera and projector simultaneously\n");
			printf("'E': Calibrate projector-camera alignment\n");
			printf("'ESC': Exit application\n");
		}

		// Get keystroke.
		cvKey = _getch();
	}

	// Release allocated resources.
	cvReleaseMat(&sl_calib.cam_intrinsic);
	cvReleaseMat(&sl_calib.cam_distortion);
	cvReleaseMat(&sl_calib.cam_extrinsic);
	cvReleaseMat(&sl_calib.proj_intrinsic);
	cvReleaseMat(&sl_calib.proj_distortion);
	cvReleaseMat(&sl_calib.proj_extrinsic);
	cvReleaseMat(&sl_calib.cam_center);
	cvReleaseMat(&sl_calib.proj_center);
	cvReleaseMat(&sl_calib.cam_rays);
	cvReleaseMat(&sl_calib.proj_rays);
	cvReleaseMat(&sl_calib.proj_column_planes);
	cvReleaseMat(&sl_calib.proj_row_planes);
	cvReleaseImage(&proj_frame);
	cvReleaseMat(&sl_calib.background_depth_map);
	cvReleaseImage(&sl_calib.background_image);
	cvReleaseImage(&sl_calib.background_mask);

	// Exit without errors.
	cvReleaseCapture(&capture);
	cvDestroyWindow("projWindow");
	return 0;
}