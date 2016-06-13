#include "stdafx.h"
#include "GazeVisualizationUI.h"

using namespace gt;

GazeVisualizationUI::GazeVisualizationUI(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setStyleSheet("QMainWindow {background:transparent;}");
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	m_Toolbox = new GazeVisualizationToolbox();
	QObject::connect(m_Toolbox, SIGNAL(quitApp()), this, SLOT(close()));
	QObject::connect(m_Toolbox, SIGNAL(showGazePosToggled(bool)), this, SLOT(showGazePosChanged(bool)));

}

GazeVisualizationUI::~GazeVisualizationUI()
{
	delete m_Toolbox;
}

void GazeVisualizationUI::DrawGazePoint(const GazeData& data)
{
	m_CurrentGazeData = data;
	m_ClearPainter = false;
	update();
}

void GazeVisualizationUI::ClearCurrentGazePoint()
{
	m_ClearPainter = true;
	update();
}

void GazeVisualizationUI::paintEvent(QPaintEvent * event)
{
	static QColor col = QColor(0, 250, 0, 200);
	static int ellipseWidth = 30, ellipseHeight = 15;

	QPainter painter(this);
	painter.setPen(QPen(QBrush(col), 1, Qt::PenStyle::DotLine));
	painter.setBrush(QBrush(col, Qt::BrushStyle::Dense6Pattern));
	if(!m_ClearPainter && m_ShowGazePos)
	{
		painter.drawEllipse(m_CurrentGazeData.gazePosition.x, m_CurrentGazeData.gazePosition.y, ellipseWidth, ellipseHeight);
	}

	/************** DEBUG ***************/		
	if (m_ShowGazePos)
	{
		static int radius = 20;
		painter.drawEllipse(0 - radius / 2, 0 - radius / 2, radius, radius); // left top
		painter.drawEllipse(0 - radius / 2, height() - radius / 2, radius, radius); // left bottom
		painter.drawEllipse(width() - radius / 2, 0 - radius / 2, radius, radius); // right top
		painter.drawEllipse(width() - radius / 2, height() - radius / 2, radius, radius); // right bottom
	}
	/************** DEBUG ***************/ 
}

void GazeVisualizationUI::show()
{
	QWidget::show();
	showFullScreen();
	m_Toolbox->show();
	GazeTrackerManager::GetInstance()->SetActiveState(GazeTrackerState::Running);
	GazeTrackerManager::GetInstance()->Start();
}
void GazeVisualizationUI::close()
{
	QWidget::close();
	GazeTrackerManager::GetInstance()->Stop();
}

void GazeVisualizationUI::showGazePosChanged(bool var)
{
	m_ShowGazePos = var;
	update();
}
