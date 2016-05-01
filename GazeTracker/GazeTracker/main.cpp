#include "stdafx.h"


/* Global variables */
cv::String face_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_frontalface_alt.xml";
cv::String eyes_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_eye_tree_eyeglasses.xml";
//cv::String left_eye_cascade_name = "cascade_classifier/haarcascades/other/leftEye.xml";
//cv::String right_eye_cascade_name = "cascade_classifier/haarcascades/other/rightEye.xml";

FaceDetection faceDetection = FaceDetection(face_cascade_name, eyes_cascade_name);

int main(void)
{
	HWND hwndDesktop = GetDesktopWindow();
	Camera camera = Camera(0, 800, 600);
	auto webCamCap = camera.GetCamera(); 
	//webCamCap->set(CV_CAP_PROP_FPS, 60); //NOT SUPPORTED
	cv::VideoWriter video;

	cv::Mat screen = ScreenCapture::hwnd2Mat(hwndDesktop);
	video.open("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 2, cv::Size(screen.size().width, screen.size().height), true);
	
	CV_Assert(video.isOpened());
	CV_Assert(webCamCap->isOpened());

	while (webCamCap->isOpened())
	{
		cv::Mat frame = camera.GetFrame();
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}
		//cv::imshow("camera test", frame);

		//faceDetection.test_detectAndDraw_v1(frame, 1, false);
		faceDetection.test_detectAndDraw_v3(frame);

		//video is too fast (no webCamCap->get(CV_CAP_PROP_FPS) supported for my webcam! -> search for options and check code fps)
		screen = ScreenCapture::hwnd2Mat(hwndDesktop);
		video << screen;

		int c = cv::waitKey(10);
		if ((char)c == 27) { break; } // escape

	}
	//video.release();
	return 0;
}