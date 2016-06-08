#pragma once
#include "ui_cornerconfigurationui.h"

namespace gt
{
	class CornerConfigurationUI : public QWidget
	{
		Q_OBJECT

	public:
		CornerConfigurationUI(QWidget* parent = Q_NULLPTR);
		~CornerConfigurationUI();

		signals:
		void configurationSuccess();
	private slots:
		void saveTopLeft();
		void saveTopMiddle();
		void saveTopRight();
		void saveMiddleLeft();
		void saveMiddleMiddle();
		void saveMiddleRight();
		void saveBottomLeft();
		void saveBottomMiddle();
		void saveBottomRight();
	private:
		Ui::CornerConfigurationUI ui;
	};
}

