#pragma once
#include "Singleton.h"
#include "stdafx.h"

class UISystem : public Singleton<UISystem>
{
	friend class Singleton <UISystem>;
public:
	UISystem();
	~UISystem();

	StartUI* GetStartUI();
	ConfigurationUI* GetConfigurationUI();
	TemplateConfigUI* GetEyeTemplateConfigurationUI();
	CornerConfigurationUI* GetCornerConfigurationUI();

private:
	QWidget* m_StartUI = nullptr;
	QWidget* m_ConfigurationUI = nullptr;
	QWidget* m_EyeTemplateConfigurationUI = nullptr;
	QWidget* m_CornerConfigurationUI = nullptr;
};

