#include <stdafx.h>
#include "startui.hpp"

StartUI::StartUI(QWidget * parent) : QWidget(parent) , m_RecordData(false){
	ui.setupUi(this);
	QObject::connect(ui.quitButton, SIGNAL(clicked()), this, SLOT(QApplication::quit()));
	QObject::connect(ui.aboutButton, SIGNAL(clicked()), this, SLOT(showAbout()));
	QObject::connect(ui.recordDataCheckBox, SIGNAL(stateChanged(int)), this, SLOT(recordDataChanged(int)));
	QObject::connect(ui.startButton, SIGNAL(clicked()), this, SLOT(startApplication()));
}

StartUI::~StartUI() {
	
}

void StartUI::startApplication()
{
	GazeTrackerMain::GetInstance()->Start();
}

void StartUI::showAbout()
{	
	QMessageBox::information(this, tr("GazeTracker v1.0"), tr("This application was made by Michael Pichler and Bernhard Rieder."));	
}

void StartUI::recordDataChanged(int state)
{
	m_RecordData = state == Qt::Checked;
}
