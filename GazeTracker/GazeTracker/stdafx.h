/*	open cv core modules	*/
#include <opencv2/core.hpp> //Core functionality
#include <opencv2/imgproc.hpp> //Image processing
#include <opencv2/imgcodecs.hpp> //Image file reading and writing
#include <opencv2/videoio.hpp> //Media I/O
#include <opencv2/highgui.hpp> //High-level GUI
#include <opencv2/video.hpp> //Video Analysis
#include <opencv2/calib3d.hpp> //Camera Calibration and 3D Reconstruction
#include <opencv2/features2d.hpp> //2D Features Framework
#include <opencv2/objdetect.hpp> //Object Detection
#include <opencv2/ml.hpp> //Machine Learning
#include <opencv2/flann.hpp> //Clustering and Search in Multi-Dimensional Spaces
#include <opencv2/photo.hpp> //Computational Photography
#include <opencv2/stitching.hpp> //Images stitching
//cuda files not included -> not sure if we need this
#include <opencv2/shape.hpp> //Shape Distance and Matching
#include <opencv2/superres.hpp> //Super Resolution
#include <opencv2/videostab.hpp> //Video Stabilization

/*	open cv extra modules	*/

//other
#include <Windows.h>
#include <iostream>
#include <string>

#include "Camera.h"
#include "FaceDetection.h"
#include "ScreenCapture.h"