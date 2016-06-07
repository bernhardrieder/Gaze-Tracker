#include <stdafx.h>
#include "ConfigurationUI.hpp"

#define COLOR_RED "red"
#define COLOR_GREEN "green"

ConfigurationUI::ConfigurationUI(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	changeColor(ui.webCamLabel, COLOR_RED);
	changeColor(ui.faceLabel, COLOR_RED);
	changeColor(ui.eyeTemplateConfigButton, COLOR_RED);
	changeColor(ui.templateMethodButton, COLOR_RED);
	changeColor(ui.cornerConfigButton, COLOR_RED);

	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(closeApplication()));
	QObject::connect(ui.eyeTemplateConfigButton, SIGNAL(clicked()), this, SLOT(openEyeTemplateConfig()));
	QObject::connect(ui.templateMethodButton, SIGNAL(clicked()), this, SLOT(openTemplateMethodSelection()));
	QObject::connect(ui.cornerConfigButton, SIGNAL(clicked()), this, SLOT(openCornerConfig()));
	QObject::connect(UISystem::GetInstance()->GetEyeTemplateConfigurationUI(), SIGNAL(configurationSuccess()), this, SLOT(eyeTemplateConfigSuccess())); 


	m_Thread = new QThread();
	m_StateWorker = new ConfigurationUI_StateWorker();
	m_StateWorker->moveToThread(m_Thread);
	QObject::connect(m_StateWorker, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
	QObject::connect(m_Thread, SIGNAL(started()), m_StateWorker, SLOT(process()));
	QObject::connect(m_StateWorker, SIGNAL(finished()), m_Thread, SLOT(quit()));
	QObject::connect(m_StateWorker, SIGNAL(finished()), m_StateWorker, SLOT(deleteLater()));
	QObject::connect(m_Thread, &QThread::finished, m_Thread, &QObject::deleteLater);
}

ConfigurationUI::~ConfigurationUI() {
	m_Thread->quit();
	m_Thread->wait();

	delete m_Thread;
	delete m_StateWorker;
}

void ConfigurationUI::show()
{
	QWidget::show(); 
	m_Thread->start();
}

void ConfigurationUI::close()
{
	QWidget::close();
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

	if (ret == QMessageBox::Yes)
	{
		m_StateWorker->StopThreads();
		GazeTracker::GetInstance()->Stop();
		qApp->quit();
	}
}

void ConfigurationUI::openEyeTemplateConfig()
{
	UISystem::GetInstance()->GetEyeTemplateConfigurationUI()->show();
}

void ConfigurationUI::openCornerConfig()
{
	UISystem::GetInstance()->GetCornerConfigurationUI()->show();
}

void ConfigurationUI::openTemplateMethodSelection()
{
}

void ConfigurationUI::stateChanged(int state) const
{
	switch(static_cast<ConfigurationUI_StateWorker::ConfigurationState>(state))
	{
		case ConfigurationUI_StateWorker::ConfigurationState::WebCamDetected:
			changeColor(ui.webCamLabel, COLOR_GREEN); 
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::FaceDetected:
			changeColor(ui.faceLabel, COLOR_GREEN);
			ui.eyeTemplateConfigButton->setEnabled(true); 
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateConfigDone:
			changeColor(ui.eyeTemplateConfigButton, COLOR_GREEN);
			ui.templateMethodButton->setEnabled(true);
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateMethodSelectionDone:
			changeColor(ui.templateMethodButton, COLOR_GREEN);
			ui.cornerConfigButton->setEnabled(true);
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::CornerConfigDone: 
			changeColor(ui.cornerConfigButton, COLOR_GREEN);
			ui.doneButton->setEnabled(true);
			break;
		default: break;
	}
}

void ConfigurationUI::eyeTemplateConfigSuccess() const
{
	stateChanged(static_cast<int>(ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateConfigDone));
}

void ConfigurationUI_StateWorker::process()
{
	while(static_cast<int>(m_configState) < static_cast<int>(ConfigurationState::FaceDetected) && !m_StopThreads)
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
		//QThread::sleep(1);
	}
	emit finished();
}
