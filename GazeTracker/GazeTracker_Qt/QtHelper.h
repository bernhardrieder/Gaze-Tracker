#pragma once
class QtHelper
{
private:
	QtHelper();
	~QtHelper();
public:
	static void ConvertMatToQImage(cv::Mat& picture, QImage& img);
};

