#include <stdafx.h>
#include "templateconfigui.hpp"

TemplateConfigUI::TemplateConfigUI(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	m_Thread = new QThread();
	m_EyesUpdateWorker = new TemplateConfigUI_EyesUpdateWorker(&ui);

	QObject::connect(ui.useButton, SIGNAL(clicked()), this, SLOT(saveTemplates()));
	QObject::connect(ui.useButton, SIGNAL(clicked()), m_EyesUpdateWorker, SLOT(Stop()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), m_EyesUpdateWorker, SLOT(Stop()));


	m_EyesUpdateWorker->moveToThread(m_Thread);
	QObject::connect(m_Thread, SIGNAL(started()), m_EyesUpdateWorker, SLOT(process()));
	QObject::connect(m_EyesUpdateWorker, SIGNAL(finished()), m_Thread, SLOT(quit()));
	//QObject::connect(m_EyeWorker, SIGNAL(finished()), m_EyeWorker, SLOT(deleteLater()));
	//QObject::connect(m_Thread, &QThread::finished, m_Thread, &QObject::deleteLater);
}

TemplateConfigUI::~TemplateConfigUI() {
	m_Thread->quit();
	m_Thread->wait();
	m_Thread->deleteLater();
	m_EyesUpdateWorker->deleteLater();
}

void TemplateConfigUI::show()
{
	QWidget::show();
	m_EyesUpdateWorker->Start();
	m_Thread->start();
	
}

void TemplateConfigUI_EyesUpdateWorker::process()
{
	cv::Mat leftEyePic, rightEyePic;
	while(!m_StopProcess)
	{
		if (GazeTracker::GetInstance()->GetEyes(leftEyePic, rightEyePic))
		{
			//resize eye pics!!!
			QImage leftEye, rightEye;
			TemplateConfigUI::convertMatToQImage(leftEyePic, leftEye);
			TemplateConfigUI::convertMatToQImage(rightEyePic, rightEye);
			
			ui->leftEyePicLabel->setPixmap(QPixmap::fromImage(leftEye));
			ui->rightEyePicLabel->setPixmap(QPixmap::fromImage(rightEye));
		}
	}
	emit finished();
}
void TemplateConfigUI::saveTemplates()
{
//TODO SAVE TEMPLATES
}

//http://stackoverflow.com/questions/11543298/qt-opencv-displaying-images-on-qlabel
void TemplateConfigUI::convertMatToQImage(cv::Mat& picture, QImage& img)
{
	// 8-bits unsigned, NO. OF CHANNELS=1
	if (picture.type() == CV_8UC1)
	{
		// Set the color table (used to translate colour indexes to qRgb values)
		QVector<QRgb> colorTable;
		for (int i = 0; i<256; i++)
			colorTable.push_back(qRgb(i, i, i));
		// Copy input Mat
		const uchar *qImageBuffer = static_cast<const uchar*>(picture.data);
		// Create QImage with same dimensions as input Mat
		img = QImage(qImageBuffer, picture.cols, picture.rows, picture.step, QImage::Format_Indexed8);
		img.setColorTable(colorTable);
	}
	// 8-bits unsigned, NO. OF CHANNELS=3
	if (picture.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *qImageBuffer = static_cast<const uchar*>(picture.data);
		// Create QImage with same dimensions as input Mat
		img = QImage(qImageBuffer, picture.cols, picture.rows, picture.step, QImage::Format_RGB888);
		img = img.rgbSwapped();
	}
	else
	{
		qDebug() << "ERROR: Mat could not be converted to QImage.";
	}
}
