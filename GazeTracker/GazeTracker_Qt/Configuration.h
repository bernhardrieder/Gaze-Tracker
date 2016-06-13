#pragma once
namespace gt
{
	class Configuration : public Singleton<Configuration>
	{
		friend class Singleton<Configuration>;
	public:
		Configuration();
		~Configuration();

		struct CornerConfigurationPoints
		{
			cv::Point topLeft, top, topRight, Left, center, right, bottomLeft, bottom, bottomRight;
		};
		enum class Corners
		{
			TopLeft = 1,Top,TopRight,
			Left,Center,Right,
			BottomLeft,Bottom,BottomRight
		};
		enum class Iris
		{
			Left, Right
		};

		void SetRecordData(bool record);
		bool GetRecordData() const;
		void SetCorners(CornerConfigurationPoints corners, Iris iris);
		void SetCorner(cv::Point point, Corners corner, Iris iris);
		CornerConfigurationPoints& GetCorners(Iris iris);
		static void SetEyeTemplateResizeFactor(double factor);
		static double GetEyeTemplateResizeFactor();
		static void SetTemplateMatchingMethod(int method);
	private:
		bool m_RecordData;
		CornerConfigurationPoints m_CornersLeftIris;
		CornerConfigurationPoints m_CornersRightIris;

	};
}

