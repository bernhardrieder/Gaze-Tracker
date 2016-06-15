#pragma once
#include "GazeData.h"

namespace gt
{
	struct DataTrackingXML
	{
		cv::Size DesktopSize;
		int FPS;
		double FrameScale;
		std::string FrameFileExtension;
		int FrameCount;
		std::vector<GazeData> Data;
	};

	class DataTrackingSystem : public Singleton<DataTrackingSystem>
	{
		friend class Singleton<DataTrackingSystem>;
	public:
		DataTrackingSystem();
		~DataTrackingSystem();

		void SetStoragePath(const std::string& path);
		void OpenFile();
		void CloseFile();
		void WriteDesktopSize(const cv::Size& size);
		void WriteScreenCaptureResizeFactor(int fps, double resizeFactor);
		void WriteFramesCount(int count, const std::string& fileExtension);
		//IMPORTANT: call WriteDesktopSize and WriteScreenCaptureResizeFactor first
		void WriteGazeData(const GazeData& data);
		std::string GetFramesDirectoryName() const;
		static DataTrackingXML ReadXmlFile(cv::FileStorage fs);

		std::string DefaultStorageFolder = "C://GazeTracker//output";
	private:
		cv::FileStorage m_FileStorage;
		std::string m_StoragePath;
		std::string m_FileName;
		bool m_FirstGazeData;

		int m_FramesCount;
		std::string m_FramesFileExtension;

		std::string& getStoragePath();
	};
	static void write(cv::FileStorage& fs, const std::string& str, const GazeData& x);
	static void read(const cv::FileNode& node, GazeData& x, const GazeData& default_value = GazeData());
}
