#pragma once
namespace gt
{
	class DataTrackingSystem : public Singleton < DataTrackingSystem>
	{
		friend class Singleton<DataTrackingSystem>;
	public:
		DataTrackingSystem();
		~DataTrackingSystem();

		void SetStoragePath(const std::string& path);
		void OpenFile();
		void CloseFile();
		void WriteDesktopSize(cv::Size size);
		void WriteScreenCaptureProperties(bool screenCaptured, cv::String folderName, float resizeFactor); //
		void WriteGazeData(GazeData data);
		std::string GetFramesDirectoryName();

		std::string DefaultStorageFolder = "C://GazeTracker//output";
	private:
		cv::FileStorage m_FileStorage;
		std::string m_StoragePath;
		std::string m_FileName;

		std::string& getStoragePath();
		std::string getFileName();
	};

}