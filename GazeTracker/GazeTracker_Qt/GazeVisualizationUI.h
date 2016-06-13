#pragma once

#include "ui_gazevisualizationui.h"
#include "GazeVisualizationToolbox.hpp"

namespace gt
{
	class GazeVisualizationUI : public QMainWindow
	{
		Q_OBJECT

	public:
		GazeVisualizationUI(QWidget* parent = 0);
		~GazeVisualizationUI();

		void DrawGazePoint(const GazeData& data);
		void ClearCurrentGazePoint();
		
	public slots:
		void show();
		void close();
		void showGazePosChanged(bool);

	protected:
		void paintEvent(QPaintEvent* event) override;

	private:
		Ui::GazeTracker_UIClass ui;
		GazeData m_CurrentGazeData;
		bool m_ClearPainter;
		GazeVisualizationToolbox* m_Toolbox;
		bool m_ShowGazePos = true;
	};
}

