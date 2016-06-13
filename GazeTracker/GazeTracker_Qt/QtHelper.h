#pragma once

namespace gt
{
	class QtHelper
	{
	private:
		QtHelper();
		~QtHelper();
	public:
		static void ChangeTextColor(QLabel& label, const QString& color);
		static void ChangeTextColor(QPushButton& button, const QString& color);
		static void ChangeTextColor(QRadioButton& button, const QString& color);
		static void ChangeBackgroundColor(QPushButton& btn, const QString& color);
		static bool ConvertMatToQImage(cv::Mat& picture, QImage& img);
	};
}

