#include "stdafx.h"
#include "QtHelper.h"

using namespace gt;

QtHelper::QtHelper()
{
}


QtHelper::~QtHelper()
{
}

void QtHelper::changeColor(QLabel& label, const QString& color)
{
	label.setStyleSheet("QLabel {color: " + color + ";}");
}

void QtHelper::changeColor(QPushButton& button, const QString& color)
{
	button.setStyleSheet("QPushButton {color: " + color + ";}");
}

void QtHelper::changeColor(QRadioButton& button, const QString& color)
{
	button.setStyleSheet("QRadioButton {color: " + color + ";}");
}

//http://stackoverflow.com/questions/11543298/qt-opencv-displaying-images-on-qlabel
bool QtHelper::ConvertMatToQImage(cv::Mat& picture, QImage& img)
{
	// 8-bits unsigned, NO. OF CHANNELS=1
	if (picture.type() == CV_8UC1)
	{
		// Set the color table (used to translate colour indexes to qRgb values)
		QVector<QRgb> colorTable;
		for (int i = 0; i < 256; i++)
			colorTable.push_back(qRgb(i, i, i));
		// Copy input Mat
		const uchar* qImageBuffer = static_cast<const uchar*>(picture.data);
		// Create QImage with same dimensions as input Mat
		img = QImage(qImageBuffer, picture.cols, picture.rows, picture.step, QImage::Format_Indexed8);
		img.setColorTable(colorTable);
	}
	// 8-bits unsigned, NO. OF CHANNELS=3
	else if (picture.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar* qImageBuffer = static_cast<const uchar*>(picture.data);
		// Create QImage with same dimensions as input Mat
		img = QImage(qImageBuffer, picture.cols, picture.rows, picture.step, QImage::Format_RGB888);
		img = img.rgbSwapped();
	}
	else
	{
		qDebug() << "ERROR: Mat could not be converted to QImage.";
		return false;
	}
	return true;
}

