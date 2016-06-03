#include "stdafx.h"
#include "gazetracker_ui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GazeTracker_UI gaze_tracker_ui;
	gaze_tracker_ui.show();

	FaceDetection faceDetectionSplit;
	Camera camera = Camera(0, 800, 600);
	auto webCamCap = camera.GetCamera();
	//webCamCap->set(CV_CAP_PROP_FPS, 60); //NOT SUPPORTED

	ScreenCapture screenCapture { GetDesktopWindow() };
	screenCapture.StartCapture(30);
	auto startTime = std::chrono::high_resolution_clock::now();

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

		faceDetectionSplit.detectAndDraw(frame);

		if (cv::waitKey(1) == 27) // escape
		{
			screenCapture.StopCapture();
			break;
		} 

	}

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> dt = endTime - startTime;
	return a.exec();
}
