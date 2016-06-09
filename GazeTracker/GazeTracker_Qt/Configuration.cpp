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

void Configuration::SetCorners(CornerConfigurationPoints corners, Iris iris)
{
	GetCorners(iris) = corners;
}

void Configuration::SetCorner(cv::Point point, Corners corner, Iris iris)
{
	auto corners = GetCorners(iris);
	switch(corner)
	{
		case Corners::TopLeft: corners.topLeft = point; break;
		case Corners::Top: corners.top = point; break;
		case Corners::TopRight: corners.topRight = point; break;
		case Corners::Left: corners.Left = point; break;
		case Corners::Center: corners.center = point; break;
		case Corners::Right: corners.right = point; break;
		case Corners::BottomLeft: corners.bottomLeft = point; break;
		case Corners::Bottom: corners.bottom = point; break;
		case Corners::BottomRight: corners.bottomRight = point; break;
		default: break;
	}
}

Configuration::CornerConfigurationPoints& Configuration::GetCorners(Iris iris)
{
	switch(iris)
	{
		case Iris::Left: return m_CornersLeftIris;;
		case Iris::Right: return m_CornersRightIris;;
	}
	
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

