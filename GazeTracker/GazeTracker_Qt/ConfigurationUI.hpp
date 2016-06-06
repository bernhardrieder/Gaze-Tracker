#pragma once
#include <QWidget>
#include "ui_configurationui.h"

class ConfigurationUI_StateWorker : public  QObject
{
	Q_OBJECT
public:
	enum class ConfigurationState
	{
		NONE = 0,
		WebCamDetected = 1,
		FaceDetected,
		EyeTemplateConfigDone,
		CornerConfigDone
	};
	ConfigurationUI_StateWorker() : m_StopThreads(false){};
	~ConfigurationUI_StateWorker() {};
	void StopThreads() { m_StopThreads = true; };

public slots:
	void process();
signals:
	void finished();
	void stateChanged(int);
private:
	ConfigurationState m_configState = ConfigurationState::NONE; //1 -> webcam detected, 2 -> face detected, NOT USED: (3 -> template config done, 4 -> corner config done)
	bool m_StopThreads;
};


class ConfigurationUI : public QWidget {
	Q_OBJECT
		friend class ConfigurationUI_StateWorker;
public:
	ConfigurationUI(QWidget * parent = Q_NULLPTR);
	~ConfigurationUI();

	void show();

private slots:
	void closeApplication();
	static void openEyeTemplateConfig();
	static void openCornerConfig();
	void stateChanged(int) const;
	void eyeTemplateConfigSuccess() const;

private:
	Ui::ConfigurationUI ui;
	QThread* m_Thread;
	ConfigurationUI_StateWorker* m_StateWorker;

	static void changeColor(QLabel* label, const QString& color);
	static void changeColor(QPushButton* button, const QString& color);

};