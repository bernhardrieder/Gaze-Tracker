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
	bool GetEyes(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor = 0.0, bool equalizeHist = false);
	void GetEyesWithIrisDetection(cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor = 0.0);

	void SetRecordData(bool record) { m_RecordData = record; }
	bool GetRecordData() const	{ return m_RecordData; }
	void SetEyeTemplateResizeFactor(double factor) { m_FaceDetection.eyeTemplateResizeFactor = factor; }
	double GetEyeTemplateResizeFactor() const { return m_FaceDetection.eyeTemplateResizeFactor; }
	void SetTemplateMatchingMethod(int method) { m_FaceDetection.templateMatchingMethod = method; }

private:
	bool m_stopApp;
	bool m_RecordData;
	Camera m_Camera;
	FaceDetection m_FaceDetection;
	ScreenCapture m_ScreenCapture;
};

