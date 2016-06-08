#pragma once
#include "ui_cornerconfigurationui.h"

class CornerConfigurationUI : public QWidget
{
	Q_OBJECT

public:
	CornerConfigurationUI(QWidget* parent = Q_NULLPTR);
	~CornerConfigurationUI();

private:
	Ui::CornerConfigurationUI ui;
};

