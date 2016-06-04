#ifndef GAZETRACKER_UI_H
#define GAZETRACKER_UI_H

#include <QtWidgets/QMainWindow>
#include "ui_gazetrackerui.h"

class GazeTrackerUI : public QMainWindow
{
	Q_OBJECT

public:
	GazeTrackerUI(QWidget *parent = 0);
	~GazeTrackerUI();

private:
	Ui::GazeTracker_UIClass ui;
};

#endif // GAZETRACKER_UI_H
