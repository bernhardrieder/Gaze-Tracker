#pragma once
#include "ui_templateconfigui.h"

class TemplateConfigUI : public QWidget {
	Q_OBJECT

public:
	TemplateConfigUI(QWidget * parent = Q_NULLPTR);
	~TemplateConfigUI();

private:
	Ui::TemplateConfig ui;
};
