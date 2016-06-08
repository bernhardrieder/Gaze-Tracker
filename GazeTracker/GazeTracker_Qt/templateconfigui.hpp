#pragma once
#include "ui_templateconfigui.h"
#include "EyeTemplateSelect.hpp"

namespace gt
{
	class TemplateConfigUI_EyesUpdateWorker : public QObject
	{
		Q_OBJECT

	public:
		TemplateConfigUI_EyesUpdateWorker(Ui::TemplateConfig* ui) : m_StopProcess(false), ui(ui)
		{
		}

		~TemplateConfigUI_EyesUpdateWorker()
		{
		}

		void Start()
		{
			m_StopProcess = false;
		};

		signals:
		void finished();
		void leftEyeImageShown();
		void rightEyeImageShown();
	public slots:
		void process();
		void stop();

	private:
		bool m_StopProcess;
		Ui::TemplateConfig* ui;
	};

	class TemplateConfigUI : public QWidget
	{
		Q_OBJECT

		friend class TemplateConfigUI_EyesUpdateWorker;
	public:
		TemplateConfigUI(QWidget* parent = Q_NULLPTR);
		~TemplateConfigUI();

		signals:
		void configurationSuccess();
		void onShow();

	public slots:
		void show();
		void close();
	private slots:
		void saveTemplates();
		void onLeftEyeStartRect();
		void onLeftEyeStopRect(const QRect&);
		void onLeftEyeClear();
		void onRightEyeStartRect();
		void onRightEyeStopRect(const QRect&);
		void onRightEyeClear();

	private:
		Ui::TemplateConfig ui;
		QThread* m_Thread;
		TemplateConfigUI_EyesUpdateWorker* m_EyesUpdateWorker;
		EyeTemplateSelect* m_LeftEyeTemplateSelect;
		EyeTemplateSelect* m_RightEyeTemplateSelect;
		bool m_OnLeftEyeClicked, m_OnRightEyeClicked, m_LeftEyeReady, m_RightEyeReady;
		QPixmap m_SelectedIrisLeft, m_SelectedIrisRight;

		void checkUseButton() const;
	};
}

