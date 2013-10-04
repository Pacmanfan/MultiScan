// cvScanProCam.h: Header file for structured light scanning.
//
// Overview:
//   This file defines the functions for structured light scanning. The current implementation
//   supports a single projector-camera pair. The projector-camera system must be calibrated 
//   prior to running the scanning function. A 3D point cloud and depth map are recovered by
//   optical triangulation. Three scanning modes are implemented, including: (1) encoding only 
//   the projector columns, (2) encoding only the projector rows, (3) encoding both rows and 
//   columns. Two reconstruction methods are implemented, including: (1) "ray-plane" 
//   triangulation and (2) "ray-ray" triangulation. In the former, each optical ray from the 
//   camera is intersected with the corresponding projector column and/or row. In the later,
//   the corresponding optical rays from the camera and projector are intersected; in this 
//   case, the 3D point is assigned as the closest point to the two (generally skewed) rays.
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

// Run the background capture (used to eliminate background points from reconstructions).
int runBackgroundCapture(CvCapture* capture, struct slParams* sl_params, struct slCalib* sl_calib);

// Run the structured light scanner.
int runStructuredLight(CvCapture* capture, struct slParams* sl_params, struct slCalib* sl_calib, int scan_index);