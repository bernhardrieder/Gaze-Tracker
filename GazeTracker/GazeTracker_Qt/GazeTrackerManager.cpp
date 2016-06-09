#include "stdafx.h"
#include "GazeTrackerManager.h"

using namespace gt;

GazeTrackerManager::GazeTrackerManager() : m_stopApp(true), m_Camera(0, 800, 600), m_FaceDetection(), m_ScreenCapture(GetDesktopWindow()), m_ActiveState(GazeTrackerState::Start_UI)
{
	CreateDirectory(LPWSTR(std::wstring(L"Frames").c_str()), NULL);
	CreateDirectory(LPWSTR(std::wstring(L"template_matching").c_str()), NULL);
	m_LastIrisesPositions.left = cv::Point(0, 0);
	m_LastIrisesPositions.right = cv::Point(0, 0);
}


GazeTrackerManager::~GazeTrackerManager()
{
}


void GazeTrackerManager::Start()
{
	m_stopApp = false;
	if(m_ActiveState == GazeTrackerState::Running && Configuration::GetInstance()->GetRecordData())
	{
		m_ScreenCapture.StartCapture(30);
	}
}

void GazeTrackerManager::Stop()
{
	if (m_ActiveState == GazeTrackerState::Running && Configuration::GetInstance()->GetRecordData())
	{
		m_ScreenCapture.StopCapture();
	}
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

void GazeTrackerManager::SetActiveState(GazeTrackerState state)
{
	m_ActiveState = state;
}

GazeTrackerState GazeTrackerManager::GetActiveState() const
{
	return m_ActiveState;
}

IrisesPositions GazeTrackerManager::GetLastDetectedIrisesPositions() const
{
	return IrisesPositions();
}

void GazeTrackerManager::detectIrisesPositions()
{
	if (!m_Camera.GetCamera()->isOpened()) return;
	while (!m_stopApp)
	{
		cv::Mat frame = m_Camera.GetFrame(true, true);
		std::vector<FaceDetection::FaceROI> faces;
		m_FaceDetection.CheckForFaceROIsWithCascadeClassifier(frame, faces);
		if(faces.size() > 0)
		{
			for(auto face : faces)
			{
				cv::Point leftIris, rightIris;
				m_FaceDetection.GetIrisesCenterPositions(frame, face, leftIris, rightIris);
				setLastIrisesPositions(leftIris, rightIris);
			}
		}
		cv::waitKey(1);
	}
}

void GazeTrackerManager::setLastIrisesPositions(cv::Point& left, cv::Point& right)
{
	setLastIrisPosition(left, m_LastIrisesPositions.left);
	setLastIrisPosition(right, m_LastIrisesPositions.right);
}

void GazeTrackerManager::setLastIrisPosition(cv::Point& current, cv::Point& last)
{
	static cv::Point outlierMax(10,10);
	if (current.x == 0 && current.y == 0) return;
	if((last.x == 0 && last.y == 0) ||
		//check if current isn't an outlier!! need to know the max changes -> observe!! 
		(current.x >= (last.x - outlierMax.x) && current.x <= (last.x + outlierMax.x) && 
		current.y >= (last.y - outlierMax.y) && current.y <= (last.y + outlierMax.y)))
	{
		last = current;
	}
}

cv::Point GazeTrackerManager::convertIrisesPointsToScreenPosition(int screenWidth, int screenHeight)
{
	auto leftCorners = Configuration::GetInstance()->GetCorners(Configuration::Iris::Left);
	auto rightCorners = Configuration::GetInstance()->GetCorners(Configuration::Iris::Right);
	return cv::Point();
}

