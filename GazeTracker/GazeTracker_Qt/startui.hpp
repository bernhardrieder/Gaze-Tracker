#pragma once
#include "ui_startui.h"

class StartUI : public QWidget {
	Q_OBJECT

public:
	StartUI(QWidget * parent = Q_NULLPTR);
	~StartUI();

private:
	Ui::StartUi ui;
	bool m_RecordData;

private slots:
	static void startApplication();
	void showAbout();
	void recordDataChanged(int state);
};
