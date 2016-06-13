#include "stdafx.h"
#include "GazeConverter.h"

using namespace gt;

GazeConverter::GazeConverter(cv::Size screenSize)
{
	setScreenSize(screenSize);
}

GazeConverter::~GazeConverter()
{
}

cv::Point GazeConverter::ConvertToScreenPosition(const IrisesPositions irisesPos)
{
	GazeROI leftIrisGazeROI, rightIrisGazeROI;
	getGazeROIs(irisesPos, leftIrisGazeROI, rightIrisGazeROI);

	cv::Point left = getScreenPoint(irisesPos.left, leftIrisGazeROI);
	cv::Point right = getScreenPoint(irisesPos.right, rightIrisGazeROI);
	bool useLeft = left != m_ErrorPoint;
	bool useRight = right != m_ErrorPoint;

	cv::Point result;
	if (useLeft && useRight)
	{
		result = (left + right) / 2;
		return result;
	}
	if (useLeft)
		return left;
	if (useRight)
		return right;
	
	return m_ErrorPoint;
}

bool GazeConverter::IsError(const cv::Point& pt) const
{
	return pt == m_ErrorPoint;
}


cv::Point GazeConverter::getScreenPoint(const cv::Point& irisPos,const GazeROI& gazeRoi)
{
	if (gazeRoi.area != GazeArea::OutOfScreen)
	{
		return linearInterpolation(getScreenArea(gazeRoi.area), getValueInRange0To1(gazeRoi.areaRect, irisPos));
	}
	return m_ErrorPoint;
}

cv::Point GazeConverter::linearInterpolation(const cv::Rect& rect, const Point2ld& value) const
{
	cv::Point result;
	result.x = linearInterpolation(rect.x, rect.width, value.x);
	result.y = linearInterpolation(rect.y, rect.height, value.y);
	return result;
}

int GazeConverter::linearInterpolation(int min, int max, long double value)
{
	return (min*(1 - value) + max * value);
}

long double GazeConverter::getValueInRange0To1(int min, int max, int value)
{
	static short normalizedMin = 0, normalizedMax = 1;
	long double divisor;
	divisor = max - min;
	value -= min; //to fit the divisor
	return static_cast<long double>(value) / divisor;
}

GazeConverter::Point2ld GazeConverter::getValueInRange0To1(const cv::Rect& gazeRect, cv::Point value)
{
	value.x -= gazeRect.x; //to fit the divisor
	value.y -= gazeRect.y; //to fit the divisor
	Point2ld result;
	result.x = static_cast<long double>(value.x) / static_cast<long double>(gazeRect.width);
	result.y = static_cast<long double>(value.y) / static_cast<long double>(gazeRect.height);
	return result;
}

void GazeConverter::getGazeROIs(const IrisesPositions & irisesPos, GazeROI & leftIris, GazeROI & rightIris) const
{
	auto& cornerLeftIris = Configuration::GetInstance()->GetCorners(Configuration::Iris::Left);
	auto& cornerRightIris = Configuration::GetInstance()->GetCorners(Configuration::Iris::Right);

	leftIris.area = getGazeArea(irisesPos.left, cornerLeftIris);
	rightIris.area = getGazeArea(irisesPos.right, cornerRightIris);
	createRect(leftIris, cornerLeftIris);
	createRect(rightIris, cornerRightIris);
}

GazeConverter::GazeArea GazeConverter::getGazeArea(const cv::Point& irisPos, const Configuration::CornerConfigurationPoints& corners)
{
	if (irisPos.x <= corners.center.x && irisPos.x >= corners.Left.x) //area is on left side
		return checkSideArea(irisPos, corners, GazeArea::LeftTop, GazeArea::LeftBottom);
	if (irisPos.x > corners.center.x && irisPos.x <= corners.right.x) //area is on right side
		return checkSideArea(irisPos, corners, GazeArea::RightTop, GazeArea::RightBottom);
	return GazeArea::OutOfScreen;
}

GazeConverter::GazeArea GazeConverter::checkSideArea(const cv::Point& irisPos, const Configuration::CornerConfigurationPoints& corners, GazeArea top, GazeArea bottom)
{
	if (irisPos.y < corners.center.y && irisPos.y >= corners.top.y)
		return top;
	if (irisPos.y > corners.center.y && irisPos.y <= corners.bottom.y)
		return bottom;
	return GazeArea::OutOfScreen;
}

void GazeConverter::createRect(GazeROI& roi, const Configuration::CornerConfigurationPoints& corners) const
{
	switch(roi.area)
	{
		case GazeArea::LeftTop: roi.areaRect = cv::Rect(corners.topLeft.x, corners.topLeft.y, corners.center.x - corners.Left.x, corners.center.y - corners.top.y); break;
		case GazeArea::LeftBottom: roi.areaRect = cv::Rect(corners.Left.x, corners.Left.y, corners.center.x - corners.Left.x, corners.bottom.y - corners.center.y); break;
		case GazeArea::RightTop: roi.areaRect = cv::Rect(corners.top.x, corners.top.y, corners.right.x - corners.center.x, corners.center.y - corners.top.y); break;
		case GazeArea::RightBottom: roi.areaRect = cv::Rect(corners.center.x, corners.center.y, corners.right.x - corners.center.x, corners.bottom.y - corners.center.y); break;
		case GazeArea::OutOfScreen: roi.areaRect = m_ErrorRect; break;
		default: break;
	}
}

void GazeConverter::setScreenSize(const cv::Size& screenSize)
{
	int screenWidthHalf = (screenSize.width / 2);
	int screenHeightHalf = (screenSize.height / 2);
	m_ScreenLeftBottom = cv::Rect(0, 0, screenWidthHalf, screenHeightHalf);
	m_ScreenLeftTop = cv::Rect(0, screenHeightHalf, screenWidthHalf, screenHeightHalf);
	m_ScreenRightBottom = cv::Rect(screenWidthHalf, 0, screenWidthHalf, screenHeightHalf);
	m_ScreenRightTop = cv::Rect(screenWidthHalf, screenHeightHalf, screenWidthHalf, screenHeightHalf);
}

cv::Rect& GazeConverter::getScreenArea(GazeArea area)
{
	switch (area)
	{
	case GazeArea::LeftTop: return m_ScreenLeftTop;
	case GazeArea::LeftBottom: return m_ScreenLeftBottom;
	case GazeArea::RightTop: return m_ScreenRightTop;
	case GazeArea::RightBottom: return m_ScreenRightBottom;
	case GazeArea::OutOfScreen: return m_ErrorRect;
	}
}
