#include <stdafx.h>
#include "TemplateMatchingMethodConfigUI.hpp"

void TemplateMatchingMethodConfigUI_EyesUpdateWorker::process()
{
	TemplateMatchingMethodConfigUI* config = UISystem::GetInstance()->GetTemplateMatchingMethodConfigUI();
	cv::Mat leftEyePic, rightEyePic;
	while (!m_StopProcess)
	{
		GazeTracker::GetInstance()->GetEyesWithIrisDetection(leftEyePic, rightEyePic, 5);

		if (!leftEyePic.empty())
		{
			QImage leftEye;
			QtHelper::ConvertMatToQImage(leftEyePic, leftEye);
			config->ui.eyeLeftLabel->clear();
			config->ui.eyeLeftLabel->setPixmap(QPixmap::fromImage(leftEye));
		}
		if (!rightEyePic.empty())
		{
			QImage rightEye;
			QtHelper::ConvertMatToQImage(rightEyePic, rightEye);
			config->ui.eyeRightLabel->clear();
			config->ui.eyeRightLabel->setPixmap(QPixmap::fromImage(rightEye));
		}
		emit picsAssigned();
		QThread::sleep(0.5);
	}
	emit finished();
}

TemplateMatchingMethodConfigUI::TemplateMatchingMethodConfigUI(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	m_Thread = new QThread();
	m_EyesUpdateWorker = new TemplateMatchingMethodConfigUI_EyesUpdateWorker();
	m_EyesUpdateWorker->moveToThread(m_Thread);
	QObject::connect(m_Thread, SIGNAL(started()), m_EyesUpdateWorker, SLOT(process()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(finished()), m_Thread, SLOT(quit()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(picsAssigned()), this, SLOT(changeMethod()));

	QObject::connect(ui.useButton, SIGNAL(clicked()), m_EyesUpdateWorker, SLOT(Stop()));
	QObject::connect(ui.useButton, SIGNAL(clicked()), this, SLOT(evaluateTemplateMethod()));
	QObject::connect(ui.sqdiff, SIGNAL(toggled(bool)), this, SLOT(chooseSQDIFF(bool)));
	QObject::connect(ui.sqdiff_normed, SIGNAL(toggled(bool)), this, SLOT(chooseSQDIFFNORMED(bool)));
	QObject::connect(ui.tm_corr, SIGNAL(toggled(bool)), this, SLOT(chooseTMCCORR(bool)));
	QObject::connect(ui.tm_corr_normed, SIGNAL(toggled(bool)), this, SLOT(chooseTMCCORRNORMED(bool)));
	QObject::connect(ui.tm_coeff, SIGNAL(toggled(bool)), this, SLOT(chooseTMCOEFF(bool)));
	QObject::connect(ui.tm_coeff_normed, SIGNAL(toggled(bool)), this, SLOT(chooseTMCOEFFNORMED(bool)));
}

TemplateMatchingMethodConfigUI::~TemplateMatchingMethodConfigUI() {
	m_Thread->quit();
	m_Thread->wait();
	m_Thread->deleteLater();
	m_EyesUpdateWorker->deleteLater();

	delete m_Thread;
	delete m_EyesUpdateWorker;
}

void TemplateMatchingMethodConfigUI::show()
{
	QWidget::show();
	m_EyesUpdateWorker->Start();
	m_Thread->start();
}

void TemplateMatchingMethodConfigUI::chooseSQDIFF(bool val)
{
	if(val)
		m_ChoosenMethod = 0;
}

void TemplateMatchingMethodConfigUI::chooseSQDIFFNORMED(bool val)
{
	if (val)
		m_ChoosenMethod = 1;
}

void TemplateMatchingMethodConfigUI::chooseTMCCORR(bool val)
{
	if (val)
		m_ChoosenMethod = 2;
}

void TemplateMatchingMethodConfigUI::chooseTMCCORRNORMED(bool val)
{
	if (val)
		m_ChoosenMethod = 3;
}

void TemplateMatchingMethodConfigUI::chooseTMCOEFF(bool val)
{
	if (val)
		m_ChoosenMethod = 4;
}

void TemplateMatchingMethodConfigUI::chooseTMCOEFFNORMED(bool val)
{
	if (val)
		m_ChoosenMethod = 5;
	
}

void TemplateMatchingMethodConfigUI::changeMethod()
{
	GazeTracker::GetInstance()->SetTemplateMatchingMethod(m_ChoosenMethod);
}

void TemplateMatchingMethodConfigUI::evaluateTemplateMethod()
{
	//"Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	
	if (ui.sqdiff->isChecked())
		m_ChoosenMethod = 0;
	else if (ui.sqdiff_normed->isChecked())
		m_ChoosenMethod = 1;
	else if (ui.tm_corr->isChecked())
		m_ChoosenMethod = 2;
	else if (ui.tm_corr_normed->isChecked())
		m_ChoosenMethod = 3;
	else if (ui.tm_coeff->isChecked())
		m_ChoosenMethod = 4;
	else if (ui.tm_coeff_normed->isChecked())
		m_ChoosenMethod = 5;
	emit configurationSuccess();
}
