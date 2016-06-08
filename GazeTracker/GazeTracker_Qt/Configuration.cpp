#include "stdafx.h"
#include "Configuration.h"
using namespace gt;

Configuration::Configuration(): m_RecordData(false)
{
}


Configuration::~Configuration()
{
}

void Configuration::SetRecordData(bool record)
{
	m_RecordData = record;
}

bool Configuration::GetRecordData() const
{
	return m_RecordData;
}

void Configuration::SetEyeTemplateResizeFactor(double factor)
{
	GazeTrackerManager::GetInstance()->m_FaceDetection.eyeTemplateResizeFactor = factor;
}

double Configuration::GetEyeTemplateResizeFactor()
{
	return GazeTrackerManager::GetInstance()->m_FaceDetection.eyeTemplateResizeFactor;
}

void Configuration::SetTemplateMatchingMethod(int method)
{
	GazeTrackerManager::GetInstance()->m_FaceDetection.templateMatchingMethod = method;
}

