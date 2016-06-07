#include "stdafx.h"
#include "GazeTracker.h"


GazeTracker::GazeTracker() : m_stopApp(true), m_RecordData(false), m_Camera(0, 800, 600), m_FaceDetection(), m_ScreenCapture(GetDesktopWindow())
{
}


GazeTracker::~GazeTracker() 
{
}


void GazeTracker::Start()
{
	UISystem::GetInstance()->GetStartUI()->show();
}

void GazeTracker::Stop()
{
	m_ScreenCapture.StopCapture();
	m_stopApp = true;
}

bool GazeTracker::IsCameraOpened() const
{ return m_Camera.GetCamera()->isOpened(); }

bool GazeTracker::IsFaceDetected() 
{
	std::vector<FaceDetection::FaceROI> out;
	cv::Mat frame = m_Camera.GetFrame(true, true);
	m_FaceDetection.CheckForFaceROIsWithCascadeClassifier(frame, out);
	return out.size() > 0;
}

void GazeTracker::detect()
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

	while (webCamCap->isOpened())
	{
		cv::Mat frame = camera.GetFrame(true, true);
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		faceDetectionSplit.detectFaceEyeIrisAndDraw(frame);

		if (cv::waitKey(1) == 27 || m_stopApp) // escape
		{
			screenCapture.StopCapture();
			break;
		}
	}
}

bool GazeTracker::GetEyes(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor, bool equalizeHist)
{
	if (!m_Camera.GetCamera()->isOpened()) return false;
	cv::Mat frame = m_Camera.GetFrame(true, true);
	return m_FaceDetection.GetEyes(frame, leftEye, rightEye, resizeFactor, equalizeHist);
}
