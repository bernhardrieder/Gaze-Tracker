#pragma once
#include "Singleton.h"

class GazeTracker : public Singleton<GazeTracker>
{
	friend class Singleton <GazeTracker>;
public:
	GazeTracker();
	~GazeTracker();

	static void Start();
	void Stop();

	bool IsCameraOpened() const;
	bool IsFaceDetected();
	void detect();
	bool GetEyes(cv::Mat& leftEye, cv::Mat& rightEye);

	void SetRecordData(bool record) { m_RecordData = record; }
	bool GetRecordData() const
	{ return m_RecordData; }

private:
	bool m_stopApp;
	bool m_RecordData;
	Camera m_Camera;
	FaceDetection m_FaceDetection;
	ScreenCapture m_ScreenCapture;
};

