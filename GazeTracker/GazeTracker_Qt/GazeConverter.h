#pragma once
#include "Configuration.h"

namespace gt
{
	struct IrisesPositions;

	class GazeConverter
	{
	public:
		GazeConverter(cv::Size screenSize);
		~GazeConverter();
		
		cv::Point ConvertToScreenPosition(const IrisesPositions irisesPos);
		bool IsError(const cv::Point& pt) const;

	private:
		enum class GazeArea
		{ LeftTop, LeftBottom, RightTop, RightBottom, OutOfScreen };
		struct GazeROI
		{
			cv::Rect areaRect; GazeArea area;
		};
		typedef cv::Point_<long double> Point2ld;

		cv::Point getScreenPoint(const cv::Point& irisPos, const GazeROI& gazeRoi);
		cv::Point linearInterpolationBetweenScreenAndIrisPos(const cv::Rect& rect, const Point2ld& value) const;
		static int linearInterpolation(int min, int max, long double value);
		static Point2ld getValueInRange0To1(const cv::Rect& gazeRect, cv::Point value);
		void getGazeROIs(const IrisesPositions& irisesPos, GazeROI& leftIris, GazeROI& rightIris) const;
		static GazeArea getGazeArea(const cv::Point& irisPos, const cv::Rect& corners);
		static GazeArea checkSideArea(const cv::Point& irisPos, const cv::Rect& corners, GazeArea top, GazeArea bottom);
		void fillAreaRect(GazeROI& roi, const cv::Rect& corners) const;
		void setScreenSize(const cv::Size& screenSize); 
		cv::Rect& getScreenArea(GazeArea area);

		cv::Rect m_ErrorRect = cv::Rect(0, 0, 0, 0);
		cv::Point m_ErrorPoint = cv::Point(-1, -1);
		cv::Rect m_ScreenLeftTop, m_ScreenLeftBottom, m_ScreenRightTop, m_ScreenRightBottom;
	};
}
