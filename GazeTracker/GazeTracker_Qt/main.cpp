#include "stdafx.h"
#include "GazeTracker.h"

int main(int argc, char* argv[])
{
	GazeTracker tracker(argc, argv);
	tracker.StartApplication();
	return tracker.exec();
}

