#include "stdafx.h"
#include "Configuration.h"
using namespace gt;

Configuration::Configuration(): m_RecordData(false)
{
}


Configuration::~Configuration()
{
}

void Configuration::SetRecordData(bool record)
{
	m_RecordData = record;
}

bool Configuration::GetRecordData() const
{
	return m_RecordData;
}

void Configuration::SetCorners(CornerConfigurationPoints corners)
{
	m_Corners = corners;
}

void Configuration::SetCorner(cv::Point point, Corners corner)
{
	switch(corner)
	{
		case Corners::TopLeft: m_Corners.topLeft = point; break;
		case Corners::Top: m_Corners.top = point; break;
		case Corners::TopRight: m_Corners.topRight = point; break;
		case Corners::Left: m_Corners.Left = point; break;
		case Corners::Center: m_Corners.center = point; break;
		case Corners::Right: m_Corners.right = point; break;
		case Corners::BottomLeft: m_Corners.bottomLeft = point; break;
		case Corners::Bottom: m_Corners.bottom = point; break;
		case Corners::BottomRight: m_Corners.bottomRight = point; break;
		default: break;
	}
}

Configuration::CornerConfigurationPoints& Configuration::GetCorners()
{
	return m_Corners;
}

void Configuration::SetEyeTemplateResizeFactor(double factor)
{
	GazeTrackerManager::GetInstance()->m_FaceDetection.eyeTemplateResizeFactor = factor;
}

double Configuration::GetEyeTemplateResizeFactor()
{
	return GazeTrackerManager::GetInstance()->m_FaceDetection.eyeTemplateResizeFactor;
}

void Configuration::SetTemplateMatchingMethod(int method)
{
	GazeTrackerManager::GetInstance()->m_FaceDetection.templateMatchingMethod = method;
}

