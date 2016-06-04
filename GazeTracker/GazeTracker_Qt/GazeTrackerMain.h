#pragma once
#include "startui.hpp"
#include "Singleton.h"

class GazeTrackerMain : public Singleton<GazeTrackerMain>
{
	friend class Singleton <GazeTrackerMain>;
public:
	GazeTrackerMain();
	~GazeTrackerMain();

	void Welcome();
	void Start();
	void Stop();

private:
	bool m_stopApp;
	QWidget* m_startUI;
};

