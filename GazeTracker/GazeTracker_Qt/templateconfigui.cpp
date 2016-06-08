#include <stdafx.h>
#include "templateconfigui.hpp"

TemplateConfigUI::TemplateConfigUI(QWidget * parent) : QWidget(parent), m_OnLeftEyeClicked(false), m_OnRightEyeClicked(false), m_LeftEyeReady(false), m_RightEyeReady(false) {
	ui.setupUi(this);

	m_Thread = new QThread();
	m_EyesUpdateWorker = new TemplateConfigUI_EyesUpdateWorker(&ui);
	m_LeftEyeTemplateSelect = new EyeTemplateSelect(this);
	m_RightEyeTemplateSelect = new EyeTemplateSelect(this);

	ui.leftEyeLayout->addWidget(m_LeftEyeTemplateSelect);
	ui.rightEyeLayout->addWidget(m_RightEyeTemplateSelect);

	QObject::connect(ui.useButton, SIGNAL(clicked()), this, SLOT(saveTemplates()));
	QObject::connect(ui.useButton, SIGNAL(clicked()), m_EyesUpdateWorker, SLOT(Stop()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), m_EyesUpdateWorker, SLOT(Stop()));
	QObject::connect(m_LeftEyeTemplateSelect, SIGNAL(mouseLeftClicked()), this, SLOT(onLeftEyeStartRect()));
	QObject::connect(m_LeftEyeTemplateSelect, SIGNAL(mouseLeftReleased(const QRect&)), this, SLOT(onLeftEyeStopRect(const QRect&)));
	QObject::connect(m_LeftEyeTemplateSelect, SIGNAL(mouseRightClicked()), this, SLOT(onLeftEyeClear()));
	QObject::connect(m_RightEyeTemplateSelect, SIGNAL(mouseLeftClicked()), this, SLOT(onRightEyeStartRect()));
	QObject::connect(m_RightEyeTemplateSelect, SIGNAL(mouseLeftReleased(const QRect&)), this, SLOT(onRightEyeStopRect(const QRect&)));
	QObject::connect(m_RightEyeTemplateSelect, SIGNAL(mouseRightClicked()), this, SLOT(onRightEyeClear()));
	QObject::connect(this, SIGNAL(onShow()), m_LeftEyeTemplateSelect, SLOT(reset()));
	QObject::connect(this, SIGNAL(onShow()), m_RightEyeTemplateSelect, SLOT(reset()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(leftEyeImageShown()), m_LeftEyeTemplateSelect, SLOT(imageAvailable()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(rightEyeImageShown()), m_RightEyeTemplateSelect, SLOT(imageAvailable()));

	m_EyesUpdateWorker->moveToThread(m_Thread);
	QObject::connect(m_Thread, SIGNAL(started()), m_EyesUpdateWorker, SLOT(process()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(finished()), m_Thread, SLOT(quit()));

}

TemplateConfigUI::~TemplateConfigUI() {
	m_Thread->quit();
	m_Thread->wait();
	m_Thread->deleteLater();
	m_EyesUpdateWorker->deleteLater();

	delete m_Thread;
	delete m_EyesUpdateWorker;
	delete m_LeftEyeTemplateSelect;
	delete m_RightEyeTemplateSelect;
}

void TemplateConfigUI::show()
{
	QWidget::show();
	m_EyesUpdateWorker->Start();
	m_Thread->start();
	emit onShow();
	m_OnLeftEyeClicked = false;
	m_OnRightEyeClicked = false;
	m_LeftEyeReady = false;
	m_RightEyeReady = false; 
	checkUseButton();
}

void TemplateConfigUI_EyesUpdateWorker::process()
{
	TemplateConfigUI* config = UISystem::GetInstance()->GetEyeTemplateConfigurationUI();
	GazeTracker::GetInstance()->SetEyeTemplateResizeFactor(5.0);
	cv::Mat leftEyePic, rightEyePic;
	while(!m_StopProcess)
	{
		if (GazeTracker::GetInstance()->GetEyes(leftEyePic, rightEyePic, GazeTracker::GetInstance()->GetEyeTemplateResizeFactor(), ui->equalizeHistCheckbox->checkState() == Qt::CheckState::Checked))
		{
			QImage leftEye, rightEye;
			if(!leftEyePic.empty() && !config->m_OnLeftEyeClicked)
			{
				QtHelper::ConvertMatToQImage(leftEyePic, leftEye);
				config->m_LeftEyeTemplateSelect->setPixmap(QPixmap::fromImage(leftEye));
				emit leftEyeImageShown();
			}
			if(!rightEyePic.empty() && !config->m_OnRightEyeClicked)
			{
				QtHelper::ConvertMatToQImage(rightEyePic, rightEye);
				config->m_RightEyeTemplateSelect->setPixmap(QPixmap::fromImage(rightEye));
				emit rightEyeImageShown();
			}
		}
	}
	emit finished();
}
void TemplateConfigUI::saveTemplates()
{
	m_SelectedIrisLeft.save(QString(FaceDetection::eyeLeftTemplateName.c_str()), Q_NULLPTR, 100);
	m_SelectedIrisRight.save(QString(FaceDetection::eyeRightTemplateName.c_str()), Q_NULLPTR, 100);
	emit configurationSuccess();
}

void TemplateConfigUI::onLeftEyeStartRect()
{
	m_OnLeftEyeClicked = true;
	checkUseButton();
}
void TemplateConfigUI::onLeftEyeStopRect(const QRect& rect)
{
	m_LeftEyeReady = rect.width() > 10 && rect.height() > 10 || rect.width() < -10 && rect.height() < -10;
	m_SelectedIrisLeft = m_LeftEyeTemplateSelect->pixmap()->copy(rect);
	checkUseButton();
}

void TemplateConfigUI::onLeftEyeClear()
{
	m_OnLeftEyeClicked = false;
	m_LeftEyeReady = false;
	checkUseButton();
}

void TemplateConfigUI::onRightEyeStartRect()
{
	m_OnRightEyeClicked = true;
	checkUseButton();
}
void TemplateConfigUI::onRightEyeStopRect(const QRect& rect)
{
	m_RightEyeReady = rect.width() > 10 && rect.height() > 10 || rect.width() < -10 && rect.height() < -10;
	m_SelectedIrisRight = m_RightEyeTemplateSelect->pixmap()->copy(rect);
	checkUseButton();
}
void TemplateConfigUI::onRightEyeClear()
{
	m_OnRightEyeClicked = false;
	m_RightEyeReady = false;
	checkUseButton();
}

void TemplateConfigUI::checkUseButton() const
{
	ui.useButton->setEnabled(m_LeftEyeReady && m_RightEyeReady);
}
