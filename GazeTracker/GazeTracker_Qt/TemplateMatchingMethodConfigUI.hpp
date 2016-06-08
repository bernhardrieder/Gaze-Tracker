#pragma once
#include <QWidget>
#include "ui_TemplateMatchingMethodConfigUI.h"

class TemplateMatchingMethodConfigUI_EyesUpdateWorker : public QObject
{
	Q_OBJECT

public:
	TemplateMatchingMethodConfigUI_EyesUpdateWorker() : m_StopProcess(false)
	{
	}

	~TemplateMatchingMethodConfigUI_EyesUpdateWorker()
	{
	}

	void Start()
	{
		m_StopProcess = false;
	};

public slots:
	void process();

	void Stop()
	{
		m_StopProcess = true;
	};

	signals:
	void finished();
	void picsAssigned();
private:
	bool m_StopProcess;
};

class TemplateMatchingMethodConfigUI : public QWidget
{
	Q_OBJECT

	friend class TemplateMatchingMethodConfigUI_EyesUpdateWorker;
public:
	TemplateMatchingMethodConfigUI(QWidget* parent = Q_NULLPTR);
	~TemplateMatchingMethodConfigUI();
public slots:
	void show();
	void close();
	signals:
	void configurationSuccess();
private slots:
	void evaluateTemplateMethod();
	void chooseSQDIFF(bool);
	void chooseSQDIFFNORMED(bool);
	void chooseTMCCORR(bool);
	void chooseTMCCORRNORMED(bool);
	void chooseTMCOEFF(bool);
	void chooseTMCOEFFNORMED(bool);
	void changeMethod();
private:
	Ui::TemplateMatchingMethodConfig ui;
	QThread* m_Thread;
	TemplateMatchingMethodConfigUI_EyesUpdateWorker* m_EyesUpdateWorker;

	void setTemplateMethodAndButton(bool val, int method, QRadioButton& button);
	QRadioButton* getRadioButtonForTemplateMethod(int method) const;
	int m_ChoosenMethod = 0;
};

