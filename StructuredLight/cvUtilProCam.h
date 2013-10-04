// cvUtilProCam.h: Header file defining auxiliary functions for structured lighting.
//
// Overview:
//   This file defines auxiliary functions for implementing structured lighting. Functions
//   include basic operations, such as the base-2 logarithm, as well as geometric algorithms, 
//   including fitting planes to 3D points and intersecting lines with other lines and planes.
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

// Calculate the base 2 logarithm.
double log2(double x);

// Fit a hyperplane to a set of ND points.
void cvFitPlane(const CvMat* points, float* plane);

// Find intersection between a 3D plane and a 3D line.
void intersectLineWithPlane3D(const float* q, const float* v, const float* w, float* p, float& depth);

// Find closest point to two 3D lines.
void intersectLineWithLine3D(const float* q1, const float* v1, const float* q2, const float* v2, float* p);

// Define camera capture (support Logitech QuickCam 9000 raw-mode).
IplImage* cvQueryFrame2(CvCapture* capture, struct slParams* sl_params, bool return_raw = false);

// Define "safe" camera capture.
// Note: This function will capture a single snapshot after a given instant.
IplImage* cvQueryFrameSafe(CvCapture* capture, struct slParams* sl_params, bool return_raw = false);

// Capture live image stream (e.g., for adjusting object placement).
int camPreview(CvCapture* capture, struct slParams* sl_params, struct slCalib* sl_calib);

// Shade a grayscale image using the "winter" colormap (similar to Matlab's).  
void colorizeWinter(IplImage* src, IplImage*& dst, IplImage* mask);

// Show an image, resampled to desired size.
void cvShowImageResampled(char* name, IplImage* image, int width, int height);

// Save a VRML-formatted point cloud.
int savePointsVRML(char* filename, CvMat* points, CvMat* normals, CvMat* colors, CvMat* mask);

// Save XML-formatted configuration file.
void writeConfiguration(const char* filename, struct slParams* sl_params);

// Read XML-formatted configuration file.
void readConfiguration(const char* filename, struct slParams* sl_params);

// In-place conversion of a 10-bit raw image to an 8-bit BGR image.
// Note: Only works with Logitech QuickCam 9000 in 10-bit raw-mode (with a Bayer BGGR mosaic).
void cvCvtLogitech9000Raw(IplImage* image, bool return_raw);