#ifndef GAZETRACKER_SHOWCASE_H
#define GAZETRACKER_SHOWCASE_H

#include <QtWidgets/QWidget>
#include "ui_gazetracker_showcase.h"

class GazeTracker_Showcase : public QWidget
{
	Q_OBJECT

public:
	GazeTracker_Showcase(QWidget *parent = 0);
	~GazeTracker_Showcase();

private:
	Ui::GazeTracker_ShowcaseClass ui;
};

#endif // GAZETRACKER_SHOWCASE_H
