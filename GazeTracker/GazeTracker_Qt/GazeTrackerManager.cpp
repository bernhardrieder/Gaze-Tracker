#include "stdafx.h"
#include "GazeTrackerManager.h"

using namespace gt;

GazeTrackerManager::GazeTrackerManager() : m_stopApp(true), m_Camera(0, 800, 600), m_FaceDetection(), m_ScreenCapture(GetDesktopWindow())
{
	CreateDirectory(LPWSTR(std::wstring(L"Frames").c_str()), NULL);
	CreateDirectory(LPWSTR(std::wstring(L"template_matching").c_str()), NULL);
}


GazeTrackerManager::~GazeTrackerManager()
{
}


void GazeTrackerManager::Start()
{
	UISystem::GetInstance()->GetStartUI()->show();
}

void GazeTrackerManager::Stop()
{
	m_ScreenCapture.StopCapture();
	m_stopApp = true;
}

bool GazeTrackerManager::IsCameraOpened() const
{
	return m_Camera.GetCamera()->isOpened();
}

bool GazeTrackerManager::IsFaceDetected()
{
	std::vector<FaceDetection::FaceROI> out;
	cv::Mat frame = m_Camera.GetFrame(true, true);
	m_FaceDetection.CheckForFaceROIsWithCascadeClassifier(frame, out);
	return out.size() > 0;
}

void GazeTrackerManager::detect()
{
	m_stopApp = false;
	FaceDetection faceDetectionSplit;
	faceDetectionSplit.eyeTemplateResizeFactor = 5;
	Camera camera = Camera(0, 800, 600);
	auto webCamCap = camera.GetCamera();

	ScreenCapture screenCapture{GetDesktopWindow()};
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

		faceDetectionSplit.DetectFaceEyeIrisAndDraw(frame);
		cv::imshow("result", frame);

		if (cv::waitKey(1) == 27 || m_stopApp) // escape
		{
			screenCapture.StopCapture();
			break;
		}
	}
}

bool GazeTrackerManager::GetEyes(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor, bool equalizeHist)
{
	if (!m_Camera.GetCamera()->isOpened()) return false;
	cv::Mat frame = m_Camera.GetFrame(true, true);
	return m_FaceDetection.GetEyes(frame, leftEye, rightEye, resizeFactor, equalizeHist);
}

void GazeTrackerManager::GetEyesWithIrisDetection(cv::Mat& leftIris, cv::Mat& rightIris, double resizeFactor)
{
	if (!m_Camera.GetCamera()->isOpened()) return;
	cv::Mat frame = m_Camera.GetFrame(true, true);
	m_FaceDetection.DetectFaceEyeIrisAndDraw(frame, false, false, true);
	m_FaceDetection.GetEyesForIrisDetection(frame, leftIris, rightIris);

	if (resizeFactor > 0 && !leftIris.empty() && !rightIris.empty())
	{
		try
		{
			cv::resize(leftIris, leftIris, cv::Size(), resizeFactor, resizeFactor);
			cv::resize(rightIris, rightIris, cv::Size(), resizeFactor, resizeFactor);
		}
		catch (...)
		{
			qDebug() << "EXCEPTION: resize error in GetEyesWithIrisDetection";
		}
	}
}

