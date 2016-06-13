#include "stdafx.h"
#include "Camera.h"
using namespace gt;

Camera::Camera(int cameraIndex, int width, int height, bool autoInit) : m_Width(width), m_Height(height), m_CameraIndex(cameraIndex)
{
	m_Camera = new cv::VideoCapture();
	if (autoInit)
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

cv::Mat Camera::GetFrame(bool inGray, bool flipped) const
{
	cv::Mat frame = GetFrame();
	if (!frame.empty())
	{
		if (inGray)
			ConvertToGray(frame);
		if (flipped)
			Flip(frame);
	}
	return frame;
}

void Camera::ConvertToGray(cv::Mat& frame) const
{
	cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
}

void Camera::Flip(cv::Mat& frame) const
{
	cv::flip(frame, frame, 1);
}

cv::Ptr<cv::VideoCapture> Camera::GetCamera() const
{
	return m_Camera;
}

bool Camera::IsInitialized() const
{
	return m_IsInitialized;
}

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

