#include <stdafx.h>
#include "TemplateMatchingMethodConfigUI.hpp"
using namespace gt;

void TemplateMatchingMethodConfigUI_EyesUpdateWorker::process()
{
	TemplateMatchingMethodConfigUI* config = UISystem::GetInstance()->GetTemplateMatchingMethodConfigUI();
	while (!m_StopProcess)
	{
		cv::Mat leftEyePic, rightEyePic;
		GazeTrackerManager::GetInstance()->GetEyesWithIrisDetection(leftEyePic, rightEyePic, 3.5);

		QImage leftEye, rightEye;

		if (!leftEyePic.empty() && QtHelper::ConvertMatToQImage(leftEyePic, leftEye) && leftEyePic.rows <= 1000 && leftEyePic.cols <= 1000 && leftEyePic.rows >= 100 && leftEyePic.cols >= 100)
		{
			config->ui.eyeLeftLabel->clear();
			config->ui.eyeLeftLabel->setPixmap(QPixmap::fromImage(leftEye));
		}
		if (!rightEyePic.empty() && QtHelper::ConvertMatToQImage(rightEyePic, rightEye) && rightEyePic.rows <= 1000 && rightEyePic.cols <= 1000  && rightEyePic.rows >= 100 && rightEyePic.cols >= 100)
		{
			config->ui.eyeRightLabel->clear();
			config->ui.eyeRightLabel->setPixmap(QPixmap::fromImage(rightEye));
		}
		emit picsAssigned();
	}
	emit finished();
}

TemplateMatchingMethodConfigUI::TemplateMatchingMethodConfigUI(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	m_Thread = new QThread(this);
	m_EyesUpdateWorker = new TemplateMatchingMethodConfigUI_EyesUpdateWorker();
	m_EyesUpdateWorker->moveToThread(m_Thread);
	QObject::connect(m_Thread, SIGNAL(started()), m_EyesUpdateWorker, SLOT(process()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(finished()), m_Thread, SLOT(quit()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(picsAssigned()), this, SLOT(changeMethod()));

	//QObject::connect(ui.useButton, SIGNAL(clicked()), m_EyesUpdateWorker, SLOT(Stop())); // NOT WORKING !!? -.-
	QObject::connect(ui.useButton, SIGNAL(clicked()), this, SLOT(evaluateTemplateMethod()));
	QObject::connect(ui.sqdiff, SIGNAL(toggled(bool)), this, SLOT(chooseSQDIFF(bool)));
	QObject::connect(ui.sqdiff_normed, SIGNAL(toggled(bool)), this, SLOT(chooseSQDIFFNORMED(bool)));
	QObject::connect(ui.tm_corr, SIGNAL(toggled(bool)), this, SLOT(chooseTMCCORR(bool)));
	QObject::connect(ui.tm_corr_normed, SIGNAL(toggled(bool)), this, SLOT(chooseTMCCORRNORMED(bool)));
	QObject::connect(ui.tm_coeff, SIGNAL(toggled(bool)), this, SLOT(chooseTMCOEFF(bool)));
	QObject::connect(ui.tm_coeff_normed, SIGNAL(toggled(bool)), this, SLOT(chooseTMCOEFFNORMED(bool)));

	QtHelper::ChangeTextColor(*getRadioButtonForTemplateMethod(0), "green");
}

TemplateMatchingMethodConfigUI::~TemplateMatchingMethodConfigUI()
{
	m_Thread->quit();
	m_Thread->wait();
	m_Thread->deleteLater();
	m_EyesUpdateWorker->deleteLater();

	delete m_EyesUpdateWorker;
}

void TemplateMatchingMethodConfigUI::show()
{
	QWidget::show();

	m_EyesUpdateWorker->Start();
	m_Thread->start();
}

void TemplateMatchingMethodConfigUI::close()
{
	m_EyesUpdateWorker->Stop();
	QWidget::close();
}

void TemplateMatchingMethodConfigUI::chooseSQDIFF(bool val)
{
	setTemplateMethodAndButton(val, 0, *getRadioButtonForTemplateMethod(0));
}

void TemplateMatchingMethodConfigUI::chooseSQDIFFNORMED(bool val)
{
	setTemplateMethodAndButton(val, 1, *getRadioButtonForTemplateMethod(1));
}

void TemplateMatchingMethodConfigUI::chooseTMCCORR(bool val)
{
	setTemplateMethodAndButton(val, 2, *getRadioButtonForTemplateMethod(2));
}

void TemplateMatchingMethodConfigUI::chooseTMCCORRNORMED(bool val)
{
	setTemplateMethodAndButton(val, 3, *getRadioButtonForTemplateMethod(3));
}

void TemplateMatchingMethodConfigUI::chooseTMCOEFF(bool val)
{
	setTemplateMethodAndButton(val, 4, *getRadioButtonForTemplateMethod(4));
}

void TemplateMatchingMethodConfigUI::chooseTMCOEFFNORMED(bool val)
{
	setTemplateMethodAndButton(val, 5, *getRadioButtonForTemplateMethod(5));
}

void TemplateMatchingMethodConfigUI::changeMethod()
{
	Configuration::SetTemplateMatchingMethod(m_ChoosenMethod);
	QtHelper::ChangeTextColor(*getRadioButtonForTemplateMethod(m_ChoosenMethod), "green");
}

void TemplateMatchingMethodConfigUI::setTemplateMethodAndButton(bool val, int method, QRadioButton& button)
{
	if (val)
		m_ChoosenMethod = method;
	else
		QtHelper::ChangeTextColor(button, "black");
}

QRadioButton* TemplateMatchingMethodConfigUI::getRadioButtonForTemplateMethod(int method) const
{
	switch (method)
	{
		case 0: return ui.sqdiff;
		case 1: return ui.sqdiff_normed;
		case 2: return ui.tm_corr;
		case 3: return ui.tm_corr_normed;
		case 4: return ui.tm_coeff;
		case 5: return ui.tm_coeff_normed;
	}
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

