#include "stdafx.h"
#include "QtHelper.h"


QtHelper::QtHelper()
{
}


QtHelper::~QtHelper()
{
}

//http://stackoverflow.com/questions/11543298/qt-opencv-displaying-images-on-qlabel
void QtHelper::ConvertMatToQImage(cv::Mat& picture, QImage& img)
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
