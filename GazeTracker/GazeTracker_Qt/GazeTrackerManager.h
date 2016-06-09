#pragma once
#include "Singleton.h"

namespace gt
{
	enum GazeTrackerState
	{
		Start_UI,
		Configuration_UI,
		Running
	};

	struct IrisesPositions
	{
		cv::Point left, right;
	};
	class GazeTrackerManager : public Singleton<GazeTrackerManager>
	{
		friend class Singleton<GazeTrackerManager>;
		friend class Configuration;
	public:
		GazeTrackerManager();
		~GazeTrackerManager();

		void Start();
		void Stop();

		bool IsCameraOpened() const;
		bool IsFaceDetected();
		void detect();
		bool GetEyes(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor = 0.0, bool equalizeHist = false);
		void GetEyesWithIrisDetection(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor = 0.0);
		void SetActiveState(GazeTrackerState state);
		GazeTrackerState GetActiveState() const;
		IrisesPositions GetLastDetectedIrisesPositions() const;

	private:
		bool m_stopApp;
		Camera m_Camera;
		FaceDetection m_FaceDetection;
		ScreenCapture m_ScreenCapture;
		IrisesPositions m_LastIrisesPositions;
		GazeTrackerState m_ActiveState;

		void detectIrisesPositions();
		void setLastIrisesPositions(cv::Point& left, cv::Point& right);
		static void setLastIrisPosition(cv::Point& current, cv::Point& last);
		cv::Point convertIrisesPointsToScreenPosition(int screenWidth, int screenHeight);
	};
}

