#include "stdafx.h"
#include "GazeConverter.h"

using namespace gt;

GazeConverter::GazeConverter(cv::Size screenSize)
{
	//REWORK - DONE
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
		return linearInterpolationBetweenScreenAndIrisPos(getScreenArea(gazeRoi.area), getValueInRange0To1(gazeRoi.areaRect, irisPos));
	}
	return m_ErrorPoint;
}

cv::Point GazeConverter::linearInterpolationBetweenScreenAndIrisPos(const cv::Rect& rect, const Point2ld& value) const
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
	//REWORK - DONE
	cv::Rect cornerLeftIris = Configuration::GetInstance()->GetCornersRect(Configuration::Iris::Left);
	cv::Rect cornerRightIris = Configuration::GetInstance()->GetCornersRect(Configuration::Iris::Right);

	leftIris.area = getGazeArea(irisesPos.left, cornerLeftIris);
	rightIris.area = getGazeArea(irisesPos.right, cornerRightIris);

	fillAreaRect(leftIris, cornerLeftIris);
	fillAreaRect(rightIris, cornerRightIris);
}

GazeConverter::GazeArea GazeConverter::getGazeArea(const cv::Point& irisPos, const cv::Rect& corners)
{
	//REWORK - DONE
	int left = corners.x;
	int middle = corners.x + corners.width / 2;
	int right = corners.x + corners.width;

	if (irisPos.x >= left && irisPos.x <= middle) //area is on left side
		return checkSideArea(irisPos, corners, GazeArea::LeftTop, GazeArea::LeftBottom);

	if (irisPos.x > middle && irisPos.x <= right) //area is on right side
		return checkSideArea(irisPos, corners, GazeArea::RightTop, GazeArea::RightBottom);

	return GazeArea::OutOfScreen;
}

GazeConverter::GazeArea GazeConverter::checkSideArea(const cv::Point& irisPos, const cv::Rect& corners, GazeArea topArea, GazeArea bottomArea)
{
	//REWORK - DONE
	int top = corners.y;
	int middle = corners.y + corners.height / 2;
	int bottom = corners.y + corners.height;

	if (irisPos.y >= top && irisPos.y <= middle)
		return topArea;

	if (irisPos.y > middle && irisPos.y <= bottom)
		return bottomArea;

	return GazeArea::OutOfScreen;
}

void GazeConverter::fillAreaRect(GazeROI& roi, const cv::Rect& corners) const
{
	//REWORK - DONE
	int widthHalf = corners.width / 2;
	int heightHalf = corners.height / 2;
	cv::Size size = cv::Size(widthHalf, heightHalf);

	switch(roi.area)
	{
	case GazeArea::LeftTop: roi.areaRect = cv::Rect(cv::Point(corners.x, corners.y), size); break;
	case GazeArea::LeftBottom: roi.areaRect = cv::Rect(cv::Point(corners.x, corners.y + heightHalf), size); break;
	case GazeArea::RightTop: roi.areaRect = cv::Rect(cv::Point(corners.x + widthHalf, corners.y), size); break;
	case GazeArea::RightBottom: roi.areaRect = cv::Rect(cv::Point(corners.x + widthHalf, corners.y + heightHalf), size); break;
	case GazeArea::OutOfScreen: roi.areaRect = m_ErrorRect; break;
	}
}

void GazeConverter::setScreenSize(const cv::Size& screenSize)
{
	//REWORK - DONE
	int widthHalf = (screenSize.width / 2);
	int heightHalf = (screenSize.height / 2);
	cv::Size size = cv::Size(widthHalf, heightHalf);

	m_ScreenLeftTop = cv::Rect(cv::Point(0, 0), size);
	m_ScreenLeftBottom = cv::Rect(cv::Point(0, heightHalf), size);
	m_ScreenRightTop = cv::Rect(cv::Point(widthHalf, 0), size);
	m_ScreenRightBottom = cv::Rect(cv::Point(widthHalf, heightHalf), size);
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
