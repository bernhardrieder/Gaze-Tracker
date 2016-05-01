#pragma once
class ScreenCapture
{
public:
	ScreenCapture();
	~ScreenCapture();

	static cv::Mat hwnd2Mat(HWND hwnd);
};

