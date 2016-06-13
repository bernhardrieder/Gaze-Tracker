#include <stdafx.h>
#include "GazeVisualizationToolbox.hpp"

using namespace gt; 

GazeVisualizationToolbox::GazeVisualizationToolbox(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);
	QObject::connect(ui.showGazePosCheckBox, SIGNAL(toggled(bool)), this, SLOT(showGazePos(bool)));
	QObject::connect(ui.configurationButton, SIGNAL(clicked()), this, SLOT(repeatConfiguration()));
	QObject::connect(ui.quitButton, SIGNAL(clicked()), this, SLOT(quitApplication()));
}

GazeVisualizationToolbox::~GazeVisualizationToolbox() {
	
}

void GazeVisualizationToolbox::showGazePos(bool var)
{
	emit showGazePosToggled(var);
}

void GazeVisualizationToolbox::repeatConfiguration()
{
	emit quitApp();
	close();
	UISystem::GetInstance()->GetConfigurationUI()->show();
}

void GazeVisualizationToolbox::quitApplication()
{
	GazeTrackerManager::GetInstance()->Stop();
	qApp->quit();
	emit quitApp();
}
