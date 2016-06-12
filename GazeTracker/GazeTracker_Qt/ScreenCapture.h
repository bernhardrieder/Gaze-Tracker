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
		/*uses frameSize of window*/
		void StartCapture(int fps, const float frameScale = 0.75f);

		void StopCapture()
		{
			m_stopCapture = true;
		};

		bool IsCapturing() const
		{
			return m_isCapturing;
		};

		static cv::Size GetFrameSize(HWND window);

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

		void captureThread();
		void saveFramesThread();
		static cv::Mat hwnd2Mat(HWND& hwnd);
	};
}

