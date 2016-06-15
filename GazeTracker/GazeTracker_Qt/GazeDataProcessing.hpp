#pragma once
#include "ProcessingUI.hpp"

namespace gt
{
	class GazeDataProcessing;

	enum class GazeDataProcessingState : int
	{
		OpenFile = 0,
		ReadData,
		CreateVideo,
		FinishedWithSuccess,
		FinishedWithError
	};

	class GazeTrackerVideoCreator : public QObject
	{
		Q_OBJECT

	public:
		GazeTrackerVideoCreator(GazeDataProcessing* parent) : m_Parent(parent), m_StopProcess(false)
		{
		}

		~GazeTrackerVideoCreator()
		{
		};

	public slots:
		void process();
		void stop();

		signals:
		void finished();
	private:
		GazeDataProcessing* m_Parent;
		bool m_StopProcess;

		void drawCircles(cv::Mat& frame, std::list<std::tuple<int, cv::Point>>& list, int activeFrameCount) const;
	};


	class GazeDataProcessing : public QObject
	{
		Q_OBJECT

		friend class GazeTrackerVideoCreator;


	public:
		GazeDataProcessing(QObject* parent = Q_NULLPTR);
		~GazeDataProcessing();

		void show();

		std::string InputFilename;
		std::string InputFrameDirectory;
		std::string OutputFilePath = "C:/GazeTracker/Output";
		signals:
		void stateChanged(int);
		void progress(double);
	private slots:
		void stopProcessing() const;
	private:
		ProcessingUI* m_processingUI;
		cv::FileStorage m_fileStorage;
		std::map<std::string, cv::Point> m_dataMap;
		gt::DataTrackingXML m_xmlData;
		GazeTrackerVideoCreator* m_VideoCreator;
		QThread* m_VideoCreatorThread;

		void startProcessing();
		void openFile();
		void readFile();
		void createVideo();
	};
}
