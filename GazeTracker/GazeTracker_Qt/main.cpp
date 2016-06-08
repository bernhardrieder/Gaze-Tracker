#include "stdafx.h"
#include "startui.hpp"
using namespace gt;

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	GazeTrackerManager::Start();
	return a.exec();
}

