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
	auto& current = GetCorners(iris);
	current = corners;
	createCornerRect(iris);
}

void Configuration::SetCorner(cv::Point point, Corners corner, Iris iris)
{
	auto& corners = GetCorners(iris);
	switch(corner)
	{
		case Corners::TopLeft: corners.topLeft = point; break;
		case Corners::Top: corners.top = point; break;
		case Corners::TopRight: corners.topRight = point; break;
		case Corners::Left: corners.left = point; break;
		case Corners::Center: corners.center = point; break;
		case Corners::Right: corners.right = point; break;
		case Corners::BottomLeft: corners.bottomLeft = point; break;
		case Corners::Bottom: corners.bottom = point; break;
		case Corners::BottomRight: corners.bottomRight = point; break;
		default: break;
	}
	createCornerRect(iris);
}

Configuration::CornerConfigurationPoints& Configuration::GetCorners(Iris iris)
{
	switch(iris)
	{
		case Iris::Left: return m_CornersLeftIris;
		case Iris::Right: return m_CornersRightIris;
	}
}

cv::Rect Configuration::GetCornersRect(Iris iris)
{
	return getCornerRect(iris);
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

void Configuration::createCornerRect(Iris iris)
{
	cv::Rect& rect = getCornerRect(iris);
	auto& corners = GetCorners(iris);

	int averageX = (corners.topLeft.x + corners.left.x + corners.bottomLeft.x) / 3;
	int averageY = (corners.topLeft.y + corners.top.y + corners.topRight.y) / 3;
	int averageWidth = ((corners.topRight.x - corners.topLeft.x) + (corners.right.x - corners.left.x) + (corners.bottomRight.x - corners.bottomLeft.x)) / 3;
	int averageHeight = ((corners.bottomLeft.y - corners.topLeft.y) + (corners.bottom.y - corners.top.y) + (corners.bottomRight.y - corners.topRight.y)) / 3;

	rect = cv::Rect(averageX, averageY, averageWidth, averageHeight);
}

cv::Rect& Configuration::getCornerRect(Iris iris)
{
	switch (iris)
	{
	case Iris::Left: return m_CornerRectLeftIris;
	case Iris::Right: return m_CornerRectRightIris;
	}
}
