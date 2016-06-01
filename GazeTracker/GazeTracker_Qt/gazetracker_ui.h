#ifndef GAZETRACKER_UI_H
#define GAZETRACKER_UI_H

#include <QtWidgets/QMainWindow>
#include "ui_gazetracker_ui.h"

class GazeTracker_UI : public QMainWindow
{
	Q_OBJECT

public:
	GazeTracker_UI(QWidget *parent = 0);
	~GazeTracker_UI();

private:
	Ui::GazeTracker_UIClass ui;
};

#endif // GAZETRACKER_UI_H
