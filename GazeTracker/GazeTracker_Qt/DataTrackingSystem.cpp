#include "stdafx.h"
#include "DataTrackingSystem.h"
#include <time.h>

using namespace gt;

DataTrackingSystem::DataTrackingSystem()
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
	m_FileStorage.release();
}

void DataTrackingSystem::WriteDesktopSize(cv::Size size)
{
}

void DataTrackingSystem::WriteScreenCaptureProperties(bool screenCaptured, cv::String folderName, float resizeFactor)
{
}

void DataTrackingSystem::WriteGazeData(GazeData data)
{
}

std::string DataTrackingSystem::GetFramesDirectoryName()
{
	return std::string(m_FileName + "_frames");
}

std::string DataTrackingSystem::getFileName()
{
	return m_FileName;
}

std::string & DataTrackingSystem::getStoragePath()
{
	if (!m_StoragePath.empty())
		return m_StoragePath;
	return DefaultStorageFolder;
}
