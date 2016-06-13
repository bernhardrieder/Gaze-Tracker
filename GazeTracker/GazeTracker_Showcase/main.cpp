#include "stdafx.h"
#include "gazetracker_showcase.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GazeTracker_Showcase w;
	w.show();
	return a.exec();
}
