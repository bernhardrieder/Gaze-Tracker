#include "stdafx.h"

/* Global variables */
cv::String face_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_frontalface_alt.xml";
cv::String eyes_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_eye_tree_eyeglasses.xml";
cv::String left_eye_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_lefteye_2splits.xml";
cv::String right_eye_cascade_name = "cascade_classifier/haarcascades/opencv/haarcascade_righteye_2splits.xml";

FaceDetection faceDetection = FaceDetection(face_cascade_name, eyes_cascade_name);
FaceDetection faceDetectionSplit = FaceDetection(face_cascade_name, left_eye_cascade_name, right_eye_cascade_name);

cv::Mat src;
cv::Mat src_gray;
int thresh = 100;
int max_thresh = 255;
cv::RNG rng(12345);

void thresh_callback(int, void*);

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
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_normal", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_hist", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_bright", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_filter", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_thres", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_thres2", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_normal", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_hist", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_bright", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_filter", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_thres", cv::WINDOW_FREERATIO);
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

void thresh_callback(int, void*)
{
	cv::Mat src_copy = src.clone();
	cv::Mat threshold_output;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	threshold(src_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	findContours(threshold_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	/*std::vector<std::vector<cv::Point> >hull(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(cv::Mat(contours[i]), hull[i], false);
	}*/
	cv::Mat drawing = cv::Mat::zeros(threshold_output.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, (int)i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
		//drawContours(drawing, hull, (int)i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
	}
	namedWindow("Hull demo", cv::WINDOW_AUTOSIZE);
	imshow("Hull demo", drawing);
}