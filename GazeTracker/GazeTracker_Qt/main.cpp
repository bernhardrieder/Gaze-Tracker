#include "stdafx.h"
#include "gazetrackerui.h"
#include <QtWidgets/QApplication>
#include "startui.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GazeTrackerMain::GetInstance()->Welcome();
	return a.exec();
}
