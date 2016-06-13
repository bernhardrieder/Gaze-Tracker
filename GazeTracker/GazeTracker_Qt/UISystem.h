#pragma once
#include "Singleton.h"
#include "stdafx.h"

namespace gt
{
	class UISystem : public Singleton<UISystem>
	{
		friend class Singleton<UISystem>;
	public:
		UISystem();
		~UISystem();

		StartUI* GetStartUI();
		ConfigurationUI* GetConfigurationUI();
		TemplateConfigUI* GetEyeTemplateConfigurationUI();
		CornerConfigurationUI* GetCornerConfigurationUI();
		TemplateMatchingMethodConfigUI* GetTemplateMatchingMethodConfigUI();
		GazeVisualizationUI* GetGazeTrackerUI();

	private:
		QWidget* m_StartUI = nullptr;
		QWidget* m_ConfigurationUI = nullptr;
		QWidget* m_EyeTemplateConfigurationUI = nullptr;
		QWidget* m_CornerConfigurationUI = nullptr;
		QWidget* m_TemplateMatchingMethodConfigUI = nullptr;
		QWidget* m_GazeTrackerUI = nullptr;
	};
}

