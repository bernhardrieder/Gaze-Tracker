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

private:
	bool m_stopApp;
	Camera m_Camera;
	FaceDetection m_FaceDetection;
	ScreenCapture m_ScreenCapture;
};

