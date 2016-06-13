#include "stdafx.h"
#include "DataTrackingSystem.h"
#include <time.h>

using namespace gt;

DataTrackingSystem::DataTrackingSystem() : m_FirstGazeData(false)
{
}


DataTrackingSystem::~DataTrackingSystem()
{
}

void DataTrackingSystem::SetStoragePath(const std::string& path)
{
	m_StoragePath = path;
}

void DataTrackingSystem::OpenFile()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::stringstream st; 
	st << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");

	m_FileName = std::string(getStoragePath() + "\\tracked_data_" + st.str());
	auto dir = GetFramesDirectoryName();
	std::wstring wdir;
	CreateDirectory(LPWSTR(wdir.assign(dir.begin(), dir.end()).c_str()), NULL);
	m_FileStorage.open(std::string(m_FileName + ".xml"), cv::FileStorage::WRITE);
}

void DataTrackingSystem::CloseFile()
{
	if (!m_FileStorage.isOpened()) return;
	if(m_FirstGazeData)
		m_FileStorage << "]"; //closes gazedata sequence
	m_FileStorage.release();
}

void DataTrackingSystem::WriteDesktopSize(const cv::Size& size)
{
	if (!m_FileStorage.isOpened()) return;
	m_FileStorage << "DesktopSize" << size;
}

void DataTrackingSystem::WriteScreenCaptureResizeFactor(int fps, double resizeFactor)
{
	if (!m_FileStorage.isOpened()) return;
	m_FileStorage << "TRIED_FPS" << fps;
	m_FileStorage << "ScreenCaptureResizeFactor" << resizeFactor;
}

void DataTrackingSystem::WriteGazeData(const GazeData& data)
{
	if (!m_FileStorage.isOpened()) return;
	if(!m_FirstGazeData)
	{
		m_FileStorage << "GazeData" << "[";
		m_FirstGazeData = true;
	}
	m_FileStorage << data;
}

std::string DataTrackingSystem::GetFramesDirectoryName() const
{
	return std::string(m_FileName + "_frames");
}

//void DataTrackingSystem::READTEST()
//{
//	cv::FileStorage fs;
//	fs.open("C://GazeTracker//Output//tracked_data_13-06-2016_22-52-36.xml", cv::FileStorage::READ);
//	cv::Size DesktopSize;
//	fs["DesktopSize"] >> DesktopSize;
//	int TRIED_FPS;
//	TRIED_FPS = static_cast<int>(fs["TRIED_FPS"]);
//	double ScreenCaptureResizeFactor;
//	ScreenCaptureResizeFactor = static_cast<double>(fs["ScreenCaptureResizeFactor"]);
//	cv::FileNode n = fs["GazeData"];                         // Read string sequence - Get node
//	if (n.type() == cv::FileNode::SEQ)
//	{
//		cv::FileNodeIterator it = n.begin(), it_end = n.end(); // Go through the node
//		for (; it != it_end; ++it)
//		{
//			GazeData data;
//			*it >> data;
//			data;
//		}
//	}
//}

std::string & DataTrackingSystem::getStoragePath()
{
	if (!m_StoragePath.empty())
		return m_StoragePath;
	return DefaultStorageFolder;
}

void gt::write(cv::FileStorage& fs, const std::string& str, const GazeData& x)
{
	GazeData::write(fs, str, x);
}

void gt::read(const cv::FileNode& node, GazeData& x, const GazeData& default_value)
{
	GazeData::read(node, x, default_value);
}
