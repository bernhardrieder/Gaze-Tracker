#pragma once
#include "ui_startui.h"

namespace gt
{
	class StartUI : public QWidget
	{
		Q_OBJECT

	public:
		StartUI(QWidget* parent = Q_NULLPTR);
		~StartUI();

	public slots:
		void show();
	private:
		Ui::StartUi ui;
		QFileDialog* m_FileDialog;

	private slots:
		static void startApplication();
		void showAbout();
		void recordDataChanged(int state) const;
		void pathChanged(const QString& path);
	};
}

