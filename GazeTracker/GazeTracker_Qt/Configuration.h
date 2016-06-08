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

		CornerConfigurationPoints corners;
		void SetRecordData(bool record);
		bool GetRecordData() const;
		static void SetEyeTemplateResizeFactor(double factor);
		static double GetEyeTemplateResizeFactor();
		static void SetTemplateMatchingMethod(int method);
	private:
		bool m_RecordData;
	};
}

