#include "stdafx.h"
#include "GazeDataProcessing.hpp"

using namespace gt;

void GazeTrackerVideoCreator::process()
{
	double progressPerData = 95 / static_cast<double>(m_Parent->m_xmlData.FrameCount);
	double currentProgress = 5;
	cv::VideoWriter out;
	out.open(m_Parent->OutputFilePath + "/analytics.avi", CV_FOURCC('X', 'V', 'I', 'D'), m_Parent->m_xmlData.FPS, m_Parent->m_xmlData.DesktopSize);
	if (!out.isOpened())
	{
		emit m_Parent->stateChanged(static_cast<int>(GazeDataProcessingState::FinishedWithError));
		return;
	}
	for (int count = 1; count <= m_Parent->m_xmlData.FrameCount; count++)
	{
		std::string currentFameFileName = std::to_string(count) + m_Parent->m_xmlData.FrameFileExtension;
		cv::String file = cv::String(m_Parent->InputFrameDirectory + "/" + currentFameFileName);
		cv::Mat frame = cv::imread(file, cv::IMREAD_COLOR);
		if (!frame.empty())
		{
			cv::resize(frame, frame, m_Parent->m_xmlData.DesktopSize);
			if (m_Parent->m_dataMap.find(currentFameFileName) != m_Parent->m_dataMap.end())
			{
				//mark gazepoint in frame!!
			}
			out << frame;

		}
		currentProgress += progressPerData;
		emit m_Parent->progress(currentProgress);
	}
	emit m_Parent->stateChanged(static_cast<int>(GazeDataProcessingState::FinishedWithSuccess));
	emit finished();
}

GazeDataProcessing::GazeDataProcessing(QObject * parent) : QObject(parent) {
	m_processingUI = new ProcessingUI();

	QObject::connect(this, SIGNAL(stateChanged(int)), m_processingUI, SLOT(stateChanged(int)));
	QObject::connect(this, SIGNAL(progress(double)), m_processingUI, SLOT(changeProgess(double)));

	m_VideoCreatorThread = new QThread(this);
	m_VideoCreator = new GazeTrackerVideoCreator(this);
	m_VideoCreator->moveToThread(m_VideoCreatorThread);
	QObject::connect(m_VideoCreatorThread, SIGNAL(started()), m_VideoCreator, SLOT(process()));
	QObject::connect(m_VideoCreator, SIGNAL(finished()), m_VideoCreatorThread, SLOT(quit()));	
	QObject::connect(m_VideoCreator, SIGNAL(finished()), m_VideoCreator, SLOT(deleteLater()));
	QObject::connect(m_VideoCreatorThread, &QThread::finished, m_VideoCreatorThread, &QObject::deleteLater);

}

GazeDataProcessing::~GazeDataProcessing() {
	delete m_processingUI;
	delete m_VideoCreator;
}

void GazeDataProcessing::show()
{
	m_processingUI->show();
	startProcessing();
}

void GazeDataProcessing::startProcessing()
{
	emit stateChanged(static_cast<int>(GazeDataProcessingState::OpenFile));
	openFile();
	emit progress(1);

	emit stateChanged(static_cast<int>(GazeDataProcessingState::ReadData));
	readFile();
	emit progress(5);

	if(m_dataMap.size() != 0)
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
	for(auto d : m_xmlData.Data)
	{
		m_dataMap.insert(std::make_pair(d.frameFileName, d.gazePosition));
	}
}

void GazeDataProcessing::createVideo()
{
	m_VideoCreatorThread->start();
}