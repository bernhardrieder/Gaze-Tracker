#include "stdafx.h"
#include "GazeTrackerManager.h"
using namespace gt;

GazeTrackerManager::GazeTrackerManager() : m_stopApp(true), m_Camera(0, 800, 600), m_FaceDetection(), m_ScreenCapture(GetDesktopWindow()), m_ActiveState(GazeTrackerState::Start_UI), m_GazeConverter(ScreenCapture::GetFrameSize(GetDesktopWindow()))
{
	m_LastIrisesPositions.left = cv::Point(0, 0);
	m_LastIrisesPositions.right = cv::Point(0, 0);
}


GazeTrackerManager::~GazeTrackerManager()
{
}


void GazeTrackerManager::Start()
{
	m_stopApp = false;
	if (m_ActiveState == GazeTrackerState::Running && Configuration::GetInstance()->GetRecordData())
	{
		m_ScreenCapture.StartCapture(20);
	}

	m_DetectIrisesPositionsThread = std::thread(&GazeTrackerManager::detectIrisesPositionsThread, this);
}

void GazeTrackerManager::Stop()
{
	m_stopApp = true;
	if (m_ActiveState == GazeTrackerState::Running && Configuration::GetInstance()->GetRecordData())
	{
		m_ScreenCapture.StopCapture();
	}
	if (m_DetectIrisesPositionsThread.joinable())
		m_DetectIrisesPositionsThread.join();
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
	faceDetectionSplit.ReloadTemplates();
	Camera camera = Camera(0, 800, 600);
	auto webCamCap = camera.GetCamera();

	//ScreenCapture screenCapture{GetDesktopWindow()};
	//screenCapture.StartCapture(30, 0.75f);

	//CV_Assert(webCamCap->isOpened());
	cv::namedWindow("result");
	const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	cv::createTrackbar(trackbar_label, "result", &faceDetectionSplit.templateMatchingMethod, 5);

	while (webCamCap->isOpened())
	{
		cv::Mat frame = m_Camera.GetFrame(true, true);
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		faceDetectionSplit.DetectFaceEyeIrisAndDraw(frame);
		cv::imshow("result", frame);

		if (cv::waitKey(1) == 27 || m_stopApp) // escape
		{
			//screenCapture.StopCapture();
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
	m_FaceDetection.DetectFaceEyeIrisAndDraw(frame,false, false, true);
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
	return m_LastIrisesPositions;
}

void GazeTrackerManager::ReloadEyeTemplates()
{
	m_FaceDetection.ReloadTemplates();
}

void GazeTrackerManager::detectIrisesPositionsThread()
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
			if (m_ActiveState == Running)
			{
				cv::Point gazePoint = m_GazeConverter.ConvertToScreenPosition(m_LastIrisesPositions);
				if (!m_GazeConverter.IsError(gazePoint))
				{
					double time = clock() / static_cast<double>(CLOCKS_PER_SEC);
					std::string framename = m_ScreenCapture.GetLastFrameFileName();
					GazeData data{ time, gazePoint, framename };
					UISystem::GetInstance()->GetGazeTrackerUI()->DrawGazePoint(data);
					if (Configuration::GetInstance()->GetRecordData())
						DataTrackingSystem::GetInstance()->WriteGazeData(data);
					qDebug() << std::string("x = " + std::to_string(gazePoint.x) + ", y = " + std::to_string(gazePoint.y)).c_str();
				}
				else
				{
					UISystem::GetInstance()->GetGazeTrackerUI()->ClearCurrentGazePoint();
				}
			}
		}

	}
}

void GazeTrackerManager::setLastIrisesPositions(cv::Point& left, cv::Point& right)
{
	m_LastIrisesPositions.left = left;
	m_LastIrisesPositions.right = right;
}

