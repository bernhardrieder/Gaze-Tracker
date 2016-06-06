#include <stdafx.h>
#include "ConfigurationUI.hpp"

#define COLOR_RED "red"
#define COLOR_GREEN "green"

ConfigurationUI::ConfigurationUI(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	changeColor(ui.webCamLabel, COLOR_RED);
	changeColor(ui.faceLabel, COLOR_RED);
	changeColor(ui.eyeTemplateConfigButton, COLOR_RED);
	changeColor(ui.cornerConfigButton, COLOR_RED);
	ui.eyeTemplateConfigButton->setEnabled(false);
	ui.cornerConfigButton->setEnabled(false);
	ui.doneButton->setEnabled(false);

	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(closeApplication()));
	QObject::connect(ui.eyeTemplateConfigButton, SIGNAL(clicked()), this, SLOT(openEyeTemplateConfig()));
	QObject::connect(ui.cornerConfigButton, SIGNAL(clicked()), this, SLOT(openCornerConfig()));

	m_Thread = new QThread();
	m_StateWorker = new ConfigurationUI_StateWorker();
	m_StateWorker->moveToThread(m_Thread);
	QObject::connect(m_StateWorker, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
	QObject::connect(m_Thread, SIGNAL(started()), m_StateWorker, SLOT(process()));
	QObject::connect(m_StateWorker, SIGNAL(finished()), m_Thread, SLOT(quit()));
	QObject::connect(m_StateWorker, SIGNAL(finished()), m_StateWorker, SLOT(deleteLater()));
	QObject::connect(m_Thread, SIGNAL(finished()), m_Thread, SLOT(deleteLater()));
}

ConfigurationUI::~ConfigurationUI() {
}

void ConfigurationUI::show()
{
	QWidget::show(); 
	m_Thread->start();
}

void ConfigurationUI::changeColor(QLabel* label, const QString& color)
{
	label->setStyleSheet("QLabel {color: "+ color + ";}");
}

void ConfigurationUI::changeColor(QPushButton* button, const QString& color)
{
	button->setStyleSheet("QPushButton {color: " + color + ";}");
}

void ConfigurationUI::closeApplication()
{
	int ret = QMessageBox::warning(this, tr("Close Application?"), tr("Do you really want to close the application?"), QMessageBox::Yes | QMessageBox::No);

	if(ret == QMessageBox::Yes)
		QApplication::quit();
}

void ConfigurationUI::openEyeTemplateConfig()
{
	UISystem::GetInstance()->GetEyeTemplateConfigurationUI()->show();
}

void ConfigurationUI::openCornerConfig()
{
	UISystem::GetInstance()->GetCornerConfigurationUI()->show();
}

void ConfigurationUI::stateChanged(int state)
{
	switch(static_cast<ConfigurationUI_StateWorker::ConfigurationState>(state))
	{
		case ConfigurationUI_StateWorker::ConfigurationState::NONE: break;
		case ConfigurationUI_StateWorker::ConfigurationState::WebCamDetected:
			changeColor(ui.webCamLabel, COLOR_GREEN); break;
		case ConfigurationUI_StateWorker::ConfigurationState::FaceDetected:
			changeColor(ui.faceLabel, COLOR_GREEN);
			ui.eyeTemplateConfigButton->setEnabled(true); break;
		case ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateConfigDone: break;
		case ConfigurationUI_StateWorker::ConfigurationState::CornerConfigDone: break;
		default: break;
	}
}

void ConfigurationUI_StateWorker::process()
{
	while(static_cast<int>(m_configState) < static_cast<int>(ConfigurationState::FaceDetected))
	{
		if(static_cast<int>(m_configState) < static_cast<int>(ConfigurationState::WebCamDetected) && GazeTracker::GetInstance()->IsCameraOpened())
		{
			m_configState = ConfigurationState::WebCamDetected;
			emit stateChanged(static_cast<int>(m_configState));
		}
		if(static_cast<int>(m_configState) < static_cast<int>(ConfigurationState::FaceDetected) && GazeTracker::GetInstance()->IsFaceDetected())
		{
			m_configState = ConfigurationState::FaceDetected;
			emit stateChanged(static_cast<int>(m_configState));
		}
		QThread::sleep(1);
	}
	emit finished();
}
