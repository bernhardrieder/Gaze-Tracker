#pragma once
#include "ui_templateconfigui.h"

class TemplateConfigUI_EyesUpdateWorker : public QObject
{
	Q_OBJECT
public:
	TemplateConfigUI_EyesUpdateWorker(Ui::TemplateConfig* ui): m_StopProcess(false), ui(ui)
	{}
	~TemplateConfigUI_EyesUpdateWorker() {}

	void Start() { m_StopProcess = false; };
public slots:
	void process();
	void Stop() { m_StopProcess = true; };
signals:
	void finished();
private:
	bool m_StopProcess;
	Ui::TemplateConfig* ui;
};

class TemplateConfigUI : public QWidget {
	Q_OBJECT
	friend class TemplateConfigUI_EyesUpdateWorker;
public:
	TemplateConfigUI(QWidget * parent = Q_NULLPTR);
	~TemplateConfigUI();

	void show();

signals:
	void configurationSuccess();

private slots:
	void saveTemplates();

private:
	Ui::TemplateConfig ui;
	QThread* m_Thread;
	TemplateConfigUI_EyesUpdateWorker* m_EyesUpdateWorker;

	static void convertMatToQImage(cv::Mat& pic, QImage& img);
};
