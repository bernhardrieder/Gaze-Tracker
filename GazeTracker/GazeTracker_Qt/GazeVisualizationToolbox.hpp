#pragma once
#include <QWidget>
#include "ui_gazevisualizationtoolbox.h"

namespace gt
{
	class GazeVisualizationToolbox : public QWidget {
		Q_OBJECT

	public:
		GazeVisualizationToolbox(QWidget * parent = Q_NULLPTR);
		~GazeVisualizationToolbox();

	signals:
		void quitApp();
		void showGazePosToggled(bool);
	private slots:
		void showGazePos(bool);
		void repeatConfiguration();
		void quitApplication();
	private:
		Ui::GazeVisualizationToolbox ui;
	};

}
