#pragma once

#include "ui_gazetrackerui.h"

namespace gt
{
	class GazeTrackerUI : public QMainWindow
	{
		Q_OBJECT

	public:
		GazeTrackerUI(QWidget* parent = 0);
		~GazeTrackerUI();

	private:
		Ui::GazeTracker_UIClass ui;
	};
}

