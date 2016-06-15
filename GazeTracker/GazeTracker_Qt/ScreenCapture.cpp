#include "stdafx.h"
#include "ScreenCapture.h"

using namespace gt;

ScreenCapture::ScreenCapture(HWND window) : m_window(window), m_stopCapture(true), m_isCapturing(false), m_fps(0)
{
}

ScreenCapture::~ScreenCapture()
{
	if (m_captureThread.joinable())
		m_captureThread.join();
	if (m_saveFramesThread.joinable())
		m_saveFramesThread.join();
}

void ScreenCapture::StartCapture(int fps, const cv::Size frameSize, const float frameScale)
{
	m_frameSize = frameSize;
	m_frameSize.height *= frameScale;
	m_frameSize.width *= frameScale;
	m_fps = fps;
	m_stopCapture = false;
	m_captureThread = std::thread(&ScreenCapture::captureThread, this);
	m_saveFramesThread = std::thread(&ScreenCapture::saveFramesThread, this);
}

void ScreenCapture::StartCapture(int fps, const float frameScale)
{
	if (Configuration::GetInstance()->GetRecordData())
	{
		DataTrackingSystem::GetInstance()->WriteScreenCaptureResizeFactor(fps, frameScale);
		DataTrackingSystem::GetInstance()->WriteDesktopSize(GetFrameSize(m_window));
	}
	StartCapture(fps, GetFrameSize(m_window), frameScale);
}

void ScreenCapture::StopCapture()
{
	m_stopCapture = true;
}

bool ScreenCapture::IsCapturing() const
{
	return m_isCapturing;
}

cv::Size ScreenCapture::GetFrameSize(HWND window)
{
	cv::Mat screen = hwnd2Mat(window);
	return cv::Size(screen.size().width, screen.size().height);
}

std::string ScreenCapture::GetLastFrameFileName() const
{
	return m_LastFrameFileName;
}

void ScreenCapture::captureThread()
{
	while (!m_stopCapture)
	{
		cv::Mat frame = hwnd2Mat(m_window);
		std::unique_lock<std::mutex> unique_lock(m_ThreadMutex);
		cv::resize(frame, m_lastFrame, m_frameSize);
		unique_lock.unlock();
		m_ThreadCond.notify_all();
		frame.release();
	}
}

void ScreenCapture::saveFramesThread()
{
	double msBetweenFrames = 1000 / m_fps;
	m_isCapturing = true;
	while (!m_stopCapture)
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		std::unique_lock<std::mutex> unique_lock(m_ThreadMutex);
		m_ThreadCond.wait(unique_lock);

		if (!m_lastFrame.empty())
		{
			static int count = 0;
			std::string filename = std::to_string(++count) + ".jpg";
			cv::imwrite(std::string(DataTrackingSystem::GetInstance()->GetFramesDirectoryName() +"/" + filename), m_lastFrame);
			m_LastFrameFileName = filename;
			DataTrackingSystem::GetInstance()->WriteFramesCount(count, ".jpg");
		}
		unique_lock.unlock();
		auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> dt = endTime - startTime;
		std::chrono::duration<double, std::milli> sleepTime(msBetweenFrames - dt.count());

		if (sleepTime.count() > 0)
			std::this_thread::sleep_for(sleepTime);
	}
	m_isCapturing = false;
}

//http://stackoverflow.com/questions/14148758/how-to-capture-the-desktop-in-opencv-ie-turn-a-bitmap-into-a-mat
cv::Mat ScreenCapture::hwnd2Mat(HWND& hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width;
	HBITMAP hbwindow;
	BITMAPINFOHEADER bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize; // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	height = windowsize.bottom;
	width = windowsize.right;

	cv::Mat out;
	out.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER); //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height; //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, width, height, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, out.data, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS); //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return out;
}

