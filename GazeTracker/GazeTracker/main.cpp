#include "stdafx.h"

/* Global variables */
cv::String face_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_frontalface_alt.xml";
cv::String eyes_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_eye_tree_eyeglasses.xml";
cv::String left_eye_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_lefteye_2splits.xml";
cv::String right_eye_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_righteye_2splits.xml";

FaceDetection faceDetectionSplit = FaceDetection(face_cascade_name, left_eye_cascade_name, right_eye_cascade_name);

int main(void)
{
	HWND hwndDesktop = GetDesktopWindow();
	Camera camera = Camera(0, 800, 600);
	auto webCamCap = camera.GetCamera();
	//webCamCap->set(CV_CAP_PROP_FPS, 60); //NOT SUPPORTED
	cv::VideoWriter video;

	cv::Mat screen = ScreenCapture::hwnd2Mat(hwndDesktop);
	video.open("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, cv::Size(screen.size().width, screen.size().height), true);
	
	CV_Assert(video.isOpened());
	CV_Assert(webCamCap->isOpened()); 
	
	cv::namedWindow("result");
	const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	cv::createTrackbar(trackbar_label, "result", &faceDetectionSplit.templateMatchingMethod, 5);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_normal", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_hist", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_bright", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_filter", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_thres", cv::WINDOW_AUTOSIZE);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_thres2", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_normal", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_hist", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_bright", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_filter", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_thres", cv::WINDOW_AUTOSIZE);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_thres2", cv::WINDOW_FREERATIO);
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
		//faceDetection.test_detectAndDraw_v4(frame);
		faceDetectionSplit.detectAndDraw(frame);

		//video is too fast (no webCamCap->get(CV_CAP_PROP_FPS) supported for my webcam! -> search for options and check code fps)
		screen = ScreenCapture::hwnd2Mat(hwndDesktop);
		video << screen;

		if (cv::waitKey(1) == 27) { break; } // escape

	}
	//video.release();
	return 0;
}