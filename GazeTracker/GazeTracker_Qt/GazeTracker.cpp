#include "stdafx.h"
#include "GazeTracker.h"


GazeTracker::GazeTracker(int argc, char* argv[]) : qApplication(argc, argv)
{
}


GazeTracker::~GazeTracker()
{
}

void GazeTracker::StartApplication() const
{
	gt::UISystem::GetInstance()->GetStartUI()->show();
}

void GazeTracker::StopApplication() const
{
}

bool GazeTracker::IsDetecting() const
{
	return gt::GazeTrackerManager::GetInstance()->GetActiveState() == gt::Running; //should check if application is in config or running mode
}

int GazeTracker::exec() const
{
	return qApplication.exec();
}
