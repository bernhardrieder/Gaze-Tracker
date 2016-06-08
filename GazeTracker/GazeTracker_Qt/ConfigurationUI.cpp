#include <stdafx.h>
#include "ConfigurationUI.hpp"
using namespace gt;

ConfigurationUI::ConfigurationUI(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	QtHelper::changeColor(*ui.webCamLabel, "red");
	QtHelper::changeColor(*ui.faceLabel, "red");
	QtHelper::changeColor(*ui.eyeTemplateConfigButton, "red");
	QtHelper::changeColor(*ui.templateMethodButton, "red");
	QtHelper::changeColor(*ui.cornerConfigButton, "red");

	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(closeApplication()));
	QObject::connect(ui.eyeTemplateConfigButton, SIGNAL(clicked()), this, SLOT(openEyeTemplateConfig()));
	QObject::connect(ui.templateMethodButton, SIGNAL(clicked()), this, SLOT(openTemplateMethodSelection()));
	QObject::connect(ui.cornerConfigButton, SIGNAL(clicked()), this, SLOT(openCornerConfig()));
	QObject::connect(UISystem::GetInstance()->GetEyeTemplateConfigurationUI(), SIGNAL(configurationSuccess()), this, SLOT(eyeTemplateConfigSuccess()));
	QObject::connect(UISystem::GetInstance()->GetTemplateMatchingMethodConfigUI(), SIGNAL(configurationSuccess()), this, SLOT(eyeTemplateMathingMethodConfigSuccess()));


	m_Thread = new QThread();
	m_StateWorker = new ConfigurationUI_StateWorker();
	m_StateWorker->moveToThread(m_Thread);
	QObject::connect(m_StateWorker, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
	QObject::connect(m_Thread, SIGNAL(started()), m_StateWorker, SLOT(process()));
	QObject::connect(m_StateWorker, SIGNAL(finished()), m_Thread, SLOT(quit()));
	QObject::connect(m_StateWorker, SIGNAL(finished()), m_StateWorker, SLOT(deleteLater()));
	QObject::connect(m_Thread, &QThread::finished, m_Thread, &QObject::deleteLater);
}

ConfigurationUI::~ConfigurationUI()
{
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


void ConfigurationUI::closeApplication()
{
	int ret = QMessageBox::warning(this, tr("Close Application?"), tr("Do you really want to close the application?"), QMessageBox::Yes | QMessageBox::No);

	if (ret == QMessageBox::Yes)
	{
		m_StateWorker->StopThreads();
		GazeTrackerManager::GetInstance()->Stop();
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
	UISystem::GetInstance()->GetTemplateMatchingMethodConfigUI()->show();
}

void ConfigurationUI::stateChanged(int state) const
{
	switch (static_cast<ConfigurationUI_StateWorker::ConfigurationState>(state))
	{
		case ConfigurationUI_StateWorker::ConfigurationState::WebCamDetected:
			QtHelper::changeColor(*ui.webCamLabel, "green");
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::FaceDetected:
			QtHelper::changeColor(*ui.faceLabel, "green");
			ui.eyeTemplateConfigButton->setEnabled(true);
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateConfigDone:
			QtHelper::changeColor(*ui.eyeTemplateConfigButton, "green");
			ui.templateMethodButton->setEnabled(true);
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateMethodSelectionDone:
			QtHelper::changeColor(*ui.templateMethodButton, "green");
			ui.cornerConfigButton->setEnabled(true);
			break;
		case ConfigurationUI_StateWorker::ConfigurationState::CornerConfigDone:
			QtHelper::changeColor(*ui.cornerConfigButton, "green");
			ui.doneButton->setEnabled(true);
			break;
		default: break;
	}
}

void ConfigurationUI::eyeTemplateConfigSuccess() const
{
	stateChanged(static_cast<int>(ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateConfigDone));
}

void ConfigurationUI::eyeTemplateMathingMethodConfigSuccess() const
{
	stateChanged(static_cast<int>(ConfigurationUI_StateWorker::ConfigurationState::EyeTemplateMethodSelectionDone));
}

void ConfigurationUI_StateWorker::process()
{
	while (static_cast<int>(m_configState) < static_cast<int>(ConfigurationState::FaceDetected) && !m_StopThreads)
	{
		if (static_cast<int>(m_configState) < static_cast<int>(ConfigurationState::WebCamDetected) && GazeTrackerManager::GetInstance()->IsCameraOpened())
		{
			m_configState = ConfigurationState::WebCamDetected;
			emit stateChanged(static_cast<int>(m_configState));
		}
		if (static_cast<int>(m_configState) < static_cast<int>(ConfigurationState::FaceDetected) && GazeTrackerManager::GetInstance()->IsFaceDetected())
		{
			m_configState = ConfigurationState::FaceDetected;
			emit stateChanged(static_cast<int>(m_configState));
		}
		//QThread::sleep(1);
	}
	emit finished();
}

