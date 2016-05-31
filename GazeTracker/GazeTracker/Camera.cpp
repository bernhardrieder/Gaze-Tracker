#include "stdafx.h"
#include "Camera.h"

Camera::Camera(int cameraIndex, int width, int height, bool autoInit) : m_Width(width), m_Height(height), m_CameraIndex(cameraIndex)
{
	m_Camera = new cv::VideoCapture();
	if(autoInit)
		Initialize();
}

Camera::~Camera()
{
	m_Camera->release();
}

cv::Mat Camera::GetFrame() const
{
	cv::Mat frame;
	if (!m_IsInitialized) return frame;
	m_Camera->read(frame); //>> frame;
	return frame;
}

cv::Ptr<cv::VideoCapture> Camera::GetCamera() const
{ return m_Camera; }

bool Camera::IsInitialized() const
{ return m_IsInitialized; }

bool Camera::Initialize()
{
	if (!m_Camera.get()->open(m_CameraIndex))
	{
		std::cerr << "--(!)Error opening video capture" << std::endl;
		return false;
	}
	m_Camera->set(CV_CAP_PROP_FRAME_HEIGHT, m_Height);
	m_Camera->set(CV_CAP_PROP_FRAME_WIDTH, m_Width);
	m_IsInitialized = true;
	return true;	
}

//int main_with_lut(int argc, char** argv)
//{
//	cv::VideoCapture cap;
//	// open the default camera, use something different from 0 otherwise;
//	// Check VideoCapture documentation.
//	if (!cap.open(0))
//		return 0;
//
//	uchar table[256];
//	for (int i = 0; i < 256; ++i)
//		table[i] = (uchar)(20 * (i / 20));
//
//	cv::Mat lookUpTable(1, 256, CV_8U);
//	uchar* p = lookUpTable.ptr();
//	for (int i = 0; i < 256; ++i)
//		p[i] = table[i];
//	for (;;)
//	{
//		cv::Mat frame;
//		cv::Mat outputFrame;
//		cap >> frame;
//		// OR cap.read(frame); ?
//		if (frame.empty()) break; // end of video stream
//		cv::LUT(frame, lookUpTable, outputFrame);
//		cv::cvtColor(frame, outputFrame, cv::COLOR_BGR2GRAY);
//
//		cv::Mat sobelx;
//		cv::Sobel(outputFrame, sobelx, CV_32F, 1, 0);
//		double minVal, maxVal;
//		minMaxLoc(sobelx, &minVal, &maxVal); //find minimum and maximum intensities
//		cv::Mat draw;
//		sobelx.convertTo(draw, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
//		cv::imshow("this is you, smile! :)", draw);
//		if (cv::waitKey(1) == 27) break; // stop capturing by pressing ESC 
//	}
//}