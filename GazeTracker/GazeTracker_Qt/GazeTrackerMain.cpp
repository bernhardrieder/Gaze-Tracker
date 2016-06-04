#include "stdafx.h"
#include "GazeTrackerMain.h"


GazeTrackerMain::GazeTrackerMain() : Singleton(), m_stopApp(true)
{
	m_startUI = new StartUI();
}


GazeTrackerMain::~GazeTrackerMain() 
{
}

void GazeTrackerMain::Welcome()
{
	m_startUI->show();
}

void GazeTrackerMain::Start()
{
	m_stopApp = false;
	FaceDetection faceDetectionSplit;
	Camera camera = Camera(0, 800, 600);
	auto webCamCap = camera.GetCamera();

	ScreenCapture screenCapture{ GetDesktopWindow() };
	screenCapture.StartCapture(30, 0.75f);

	CV_Assert(webCamCap->isOpened());
	cv::namedWindow("result");
	const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	cv::createTrackbar(trackbar_label, "result", &faceDetectionSplit.templateMatchingMethod, 5);
	cv::namedWindow(ToString(FaceDetection::Eye::LEFT) + "_hist", cv::WINDOW_FREERATIO);
	cv::namedWindow(ToString(FaceDetection::Eye::RIGHT) + "_hist", cv::WINDOW_FREERATIO);

	while (webCamCap->isOpened())
	{
		cv::Mat frame = camera.GetFrame();
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		faceDetectionSplit.detectAndDraw(frame);

		if (cv::waitKey(1) == 27 || m_stopApp) // escape
		{
			screenCapture.StopCapture();
			break;
		}
	}
}

void GazeTrackerMain::Stop()
{
	m_stopApp = true;
}
