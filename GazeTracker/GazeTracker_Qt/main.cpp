#include "stdafx.h"
#include "startui.hpp"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	GazeTracker::Start();
	return a.exec();
}

