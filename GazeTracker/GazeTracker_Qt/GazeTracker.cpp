#include "stdafx.h"
#include "GazeTracker.h"


GazeTracker::GazeTracker(int argc, char* argv[]) : qApplication(argc, argv)
{
	CreateDirectory(LPWSTR(std::wstring(L"C:/GazeTracker").c_str()), NULL);
	CreateDirectory(LPWSTR(std::wstring(L"C:/GazeTracker/Output").c_str()), NULL);
	CreateDirectory(LPWSTR(std::wstring(L"C:/GazeTracker/tmp").c_str()), NULL); //DONT CHANGE
	CreateDirectory(LPWSTR(std::wstring(L"C:/GazeTracker/tmp/template_matching").c_str()), NULL); //DONT CHANGE - OR CHANGE AND CHANGE DIR IN FACEDETECTION TEMPLATE MATCHING strings
	gt::DataTrackingSystem::GetInstance()->DefaultStorageFolder = "C:\\GazeTracker\\Output";
}


GazeTracker::~GazeTracker()
{
	gt::DataTrackingSystem::GetInstance()->CloseFile();
}

void GazeTracker::StartApplication() const
{
	gt::UISystem::GetInstance()->GetStartUI()->show();
	//gt::UISystem::GetInstance()->GetGazeTrackerUI()->show();
}

void GazeTracker::StopApplication() const
{
}

bool GazeTracker::IsDetecting() const
{
	return gt::GazeTrackerManager::GetInstance()->GetActiveState() == gt::Running; //should check if application is in config or running mode
}

int GazeTracker::exec() const
{
	return qApplication.exec();
}