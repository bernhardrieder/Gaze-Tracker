#pragma once

namespace gt
{
	class QtHelper
	{
	private:
		QtHelper();
		~QtHelper();
	public:
		static void changeColor(QLabel& label, const QString& color);
		static void changeColor(QPushButton& button, const QString& color);
		static void changeColor(QRadioButton& button, const QString& color);
		static bool ConvertMatToQImage(cv::Mat& picture, QImage& img);
	};
}

