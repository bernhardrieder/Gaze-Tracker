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

		void SetRecordData(bool record);
		bool GetRecordData() const;
		void SetCorners(CornerConfigurationPoints corners);
		void SetCorner(cv::Point point, Corners corner);
		CornerConfigurationPoints& GetCorners();
		static void SetEyeTemplateResizeFactor(double factor);
		static double GetEyeTemplateResizeFactor();
		static void SetTemplateMatchingMethod(int method);
	private:
		bool m_RecordData;
		CornerConfigurationPoints m_Corners;
	};
}

