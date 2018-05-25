#include "stdafx.h"
#include "GazeConverter.h"

using namespace gt;

bool GazeConverter::useFullRectForScreenPointLinearInterpolation = false;

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
	//REWORK - DONE
	GazeROI leftIrisGazeROI, rightIrisGazeROI;
	getGazeROIs(irisesPos, leftIrisGazeROI, rightIrisGazeROI);

	cv::Point left = getScreenPoint(irisesPos.left, leftIrisGazeROI);
	cv::Point right = getScreenPoint(irisesPos.right, rightIrisGazeROI);
	bool useLeft = left != m_ErrorPoint;
	bool useRight = right != m_ErrorPoint;

	if (useLeft && useRight)
		return (left + right) / 2;
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
		if(useFullRectForScreenPointLinearInterpolation)
			return linearInterpolationBetweenScreenAndIrisPos(cv::Rect(m_ScreenLeftTop.x, m_ScreenLeftTop.y, m_ScreenLeftTop.width * 2, m_ScreenLeftTop.height * 2), getValueInRange0To1(gazeRoi.areaRect, irisPos));
		return linearInterpolationBetweenScreenAndIrisPos(getScreenArea(gazeRoi.area), getValueInRange0To1(gazeRoi.areaRect, irisPos));
	}
	return m_ErrorPoint;
}

cv::Point GazeConverter::linearInterpolationBetweenScreenAndIrisPos(const cv::Rect& ScreenRect, const Point2ld IrisGazeValueIn0To1Range)
{
	//REWORK - DONE
	int x = linearInterpolation(ScreenRect.x, ScreenRect.x + ScreenRect.width, IrisGazeValueIn0To1Range.x);
	int y = linearInterpolation(ScreenRect.y, ScreenRect.y + ScreenRect.height, IrisGazeValueIn0To1Range.y);

	return cv::Point(x,y);
}

int GazeConverter::linearInterpolation(int min, int max, long double value)
{
	//SHOULD BE OK
	return (min*(1 - value)) + (max * value);
}

GazeConverter::Point2ld GazeConverter::getValueInRange0To1(const cv::Rect& gazeRect, cv::Point value)
{
	//SHOULD BE OK
	value.x -= gazeRect.x; //to fit the divisor
	value.y -= gazeRect.y; //to fit the divisor

	long double x = value.x / static_cast<long double>(gazeRect.width);
	long double y = value.y / static_cast<long double>(gazeRect.height);

	return Point2ld(x,y);
}

void GazeConverter::getGazeROIs(const IrisesPositions & irisesPos, GazeROI & leftIris, GazeROI & rightIris) const
{
	//REWORK - DONE
	cv::Rect cornerLeftIris = Configuration::GetInstance()->GetCornersRect(Configuration::Iris::Left);
	cv::Rect cornerRightIris = Configuration::GetInstance()->GetCornersRect(Configuration::Iris::Right);

	//auto& cornersLeft = Configuration::GetInstance()->GetCorners(Configuration::Iris::Left);
	//auto& cornersRight = Configuration::GetInstance()->GetCorners(Configuration::Iris::Right);

	//qDebug() << std::string("left corners rect width = " + std::to_string(cornerLeftIris.width) + ", height = " + std::to_string(cornerLeftIris.height)).c_str();
	//qDebug() << std::string("right corners rect width = " + std::to_string(cornerRightIris.width) + ", height = " + std::to_string(cornerRightIris.height)).c_str();

	if(useFullRectForScreenPointLinearInterpolation)
	{
		leftIris.areaRect = cornerLeftIris;
		rightIris.areaRect = cornerRightIris;
	}
	else
	{
		leftIris.area = getGazeArea(irisesPos.left, cornerLeftIris);
		rightIris.area = getGazeArea(irisesPos.right, cornerRightIris);

		fillAreaRect(leftIris, cornerLeftIris);
		fillAreaRect(rightIris, cornerRightIris);
	}
}

GazeConverter::GazeArea GazeConverter::getGazeArea(const cv::Point& irisPos, const cv::Rect& corners)
{
	//REWORK - DONE
	int left = corners.x;
	int middle = corners.x + corners.width / 2;
	int right = corners.x + corners.width;

	if (irisPos.x >= left && irisPos.x <= middle) //area is on left side
		return checkIfTopOrBottomArea(irisPos, corners, GazeArea::LeftTop, GazeArea::LeftBottom);

	if (irisPos.x > middle && irisPos.x <= right) //area is on right side
		return checkIfTopOrBottomArea(irisPos, corners, GazeArea::RightTop, GazeArea::RightBottom);

	return GazeArea::OutOfScreen;
}

GazeConverter::GazeArea GazeConverter::checkIfTopOrBottomArea(const cv::Point& irisPos, const cv::Rect& corners, GazeArea topArea, GazeArea bottomArea)
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
