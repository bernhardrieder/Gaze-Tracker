#pragma once
#include <thread>
#include <mutex>

namespace gt
{
	class ScreenCapture
	{
	public:
		ScreenCapture(HWND window);
		~ScreenCapture();

		void StartCapture(int fps, const cv::Size frameSize, const float frameScale = 0.75f);
		void StartCapture(int fps, const float frameScale = 0.75f);
		void StopCapture();
		bool IsCapturing() const;

		static cv::Size GetFrameSize(HWND window);
		std::string GetLastFrameFileName() const;

	private:
		HWND m_window;
		cv::Mat m_lastFrame;
		std::mutex m_ThreadMutex;
		std::condition_variable m_ThreadCond;
		std::thread m_captureThread;
		std::thread m_saveFramesThread;
		bool m_stopCapture = false;
		bool m_isCapturing = false;
		int m_fps;
		cv::Size m_frameSize;
		std::string m_LastFrameFileName;

		void captureThread();
		void saveFramesThread();
		static cv::Mat hwnd2Mat(HWND& hwnd);
	};
}

