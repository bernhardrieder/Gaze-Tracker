#include "stdafx.h"
#include "GazeDataProcessing.hpp"

using namespace gt;

void GazeTrackerVideoCreator::process()
{
	m_StopProcess = false;
	auto& xmlData = m_Parent->m_xmlData;
	double progressPerData = 95 / static_cast<double>(xmlData.FrameCount);
	double currentProgress = 5;
	cv::VideoWriter out;
	out.open(m_Parent->OutputFilePath + "/analytics.avi", CV_FOURCC('X', 'V', 'I', 'D'), xmlData.FPS, xmlData.DesktopSize);
	if (!out.isOpened())
	{
		emit m_Parent->stateChanged(static_cast<int>(GazeDataProcessingState::FinishedWithError));
		return;
	}
	std::list<std::tuple<int, cv::Point>> activePoints;
	for (int count = 1; count <= xmlData.FrameCount && !m_StopProcess; count++)
	{
		std::string currentFrameFileName = std::to_string(count) + xmlData.FrameFileExtension;
		cv::String file = cv::String(m_Parent->InputFrameDirectory + "/" + currentFrameFileName);
		cv::Mat frame = cv::imread(file, cv::IMREAD_COLOR);
		if (!frame.empty())
		{
			cv::resize(frame, frame, xmlData.DesktopSize);
			if (m_Parent->m_dataMap.find(currentFrameFileName) != m_Parent->m_dataMap.end())
			{
				activePoints.push_back(std::make_tuple(count, m_Parent->m_dataMap[currentFrameFileName]));
				m_Parent->m_dataMap.erase(currentFrameFileName);
			}
			drawCircles(frame, activePoints, count);
			out << frame;
		}
		currentProgress += progressPerData;
		emit m_Parent->progress(currentProgress);
	}

	emit m_Parent->progress(100);
	if (!m_StopProcess)
	emit m_Parent->stateChanged(static_cast<int>(GazeDataProcessingState::FinishedWithSuccess));
	else
	emit m_Parent->stateChanged(static_cast<int>(GazeDataProcessingState::FinishedWithError));
	emit finished();
}

void GazeTrackerVideoCreator::stop()
{
	m_StopProcess = true;
}

void GazeTrackerVideoCreator::drawCircles(cv::Mat& frame, std::list<std::tuple<int, cv::Point>>& list, int activeFrameCount) const
{
	static double markAlphaMax = 128. / 255.; //max alpha value for mark color
	static cv::Scalar markCol = cv::Scalar(0, 255, 0);
	if (list.size() == 0) return;
	int& fps = m_Parent->m_xmlData.FPS;
	double alphaStep = markAlphaMax / static_cast<double>(fps); //minus step for each frame -> after 1 sec (or fps value) the alpha channel = 0 and the point will be removed
	std::vector<std::tuple<int, cv::Point>> removables;
	for (auto tuple : list)
	{
		int frameCount = std::get<0>(tuple);
		int frameDiff = activeFrameCount - frameCount;
		if (frameDiff > fps)
		{
			removables.push_back(tuple);
		}
		else
		{
			double alpha = frameDiff != 0 ? markAlphaMax - alphaStep * frameDiff : markAlphaMax;
			static int radius = 20;
			cv::Mat circleFrame = frame.clone();
			cv::circle(circleFrame, std::get<1>(tuple), radius, markCol, -1);
			//cv::ellipse(circleFrame, std::get<1>(tuple), cv::Size(radius, radius / 2), 0, 0, 360, markCol, -1);
			cv::addWeighted(circleFrame, alpha, frame, 1.0 - alpha, 0.0, frame);
		}
	}
	if (removables.size() != 0)
		for (auto tuple : removables)
		{
			list.remove(tuple);
		}
}

GazeDataProcessing::GazeDataProcessing(QObject* parent) : QObject(parent)
{
	m_processingUI = new ProcessingUI();

	QObject::connect(this, SIGNAL(stateChanged(int)), m_processingUI, SLOT(stateChanged(int)));
	QObject::connect(this, SIGNAL(progress(double)), m_processingUI, SLOT(changeProgess(double)));

	m_VideoCreatorThread = new QThread(this);
	m_VideoCreator = new GazeTrackerVideoCreator(this);
	m_VideoCreator->moveToThread(m_VideoCreatorThread);
	QObject::connect(m_VideoCreatorThread, SIGNAL(started()), m_VideoCreator, SLOT(process()));
	QObject::connect(m_VideoCreator, SIGNAL(finished()), m_VideoCreatorThread, SLOT(quit()));
	QObject::connect(m_processingUI, SIGNAL(closing()), this, SLOT(stopProcessing()));
}

GazeDataProcessing::~GazeDataProcessing()
{
	m_VideoCreatorThread->deleteLater();
	m_VideoCreator->deleteLater();
	delete m_processingUI;
	delete m_VideoCreator;
}

void GazeDataProcessing::show()
{
	m_processingUI->show();
	startProcessing();
}

void GazeDataProcessing::stopProcessing() const
{
	m_VideoCreator->stop();
}

void GazeDataProcessing::startProcessing()
{
	emit stateChanged(static_cast<int>(GazeDataProcessingState::OpenFile));
	openFile();
	emit progress(1);

	emit stateChanged(static_cast<int>(GazeDataProcessingState::ReadData));
	readFile();
	emit progress(5);

	if (m_dataMap.size() != 0)
	{
		emit stateChanged(static_cast<int>(GazeDataProcessingState::CreateVideo));
		createVideo();
	}
	else
	{
		emit stateChanged(static_cast<int>(GazeDataProcessingState::FinishedWithError));
	}
}

void GazeDataProcessing::openFile()
{
	m_fileStorage.open(InputFilename, cv::FileStorage::READ);
}

void GazeDataProcessing::readFile()
{
	m_xmlData = DataTrackingSystem::ReadXmlFile(m_fileStorage);
	for (auto d : m_xmlData.Data)
	{
		m_dataMap.insert(std::make_pair(d.frameFileName, d.gazePosition));
	}
}

void GazeDataProcessing::createVideo()
{
	m_VideoCreatorThread->start();
}
