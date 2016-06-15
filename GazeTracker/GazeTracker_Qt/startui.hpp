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
		void chooseFile();
		void chooseDir();
		void chooseVideoOutputFile();
		void xmlFileSelected(const QString &file);
		void folderPathChanged(const QString& path);
		void videoOutputChanged(const QString& path);
		void startAnalytics();
	private:
		Ui::StartUi ui;
		QFileDialog* m_FileDialog;	
		bool m_xmlSet;
		bool m_frameSet;
		GazeDataProcessing* m_dataProcessing;

	private slots:
		static void startApplication();
		void showAbout();
		void recordDataChanged(int state) const;
		void pathChanged(const QString& path);

	private:
		void checkStartAnalyticsButton() const;
	};
}

