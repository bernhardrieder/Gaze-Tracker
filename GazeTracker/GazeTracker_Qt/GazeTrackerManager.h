#pragma once
#include "Singleton.h"

namespace gt
{
	class GazeTrackerManager : public Singleton<GazeTrackerManager>
	{
		friend class Singleton<GazeTrackerManager>;
		friend class Configuration;
	public:
		GazeTrackerManager();
		~GazeTrackerManager();

		static void Start();
		void Stop();

		bool IsCameraOpened() const;
		bool IsFaceDetected();
		void detect();
		bool GetEyes(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor = 0.0, bool equalizeHist = false);
		void GetEyesWithIrisDetection(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor = 0.0);

	private:
		bool m_stopApp;
		Camera m_Camera;
		FaceDetection m_FaceDetection;
		ScreenCapture m_ScreenCapture;
	};
}

