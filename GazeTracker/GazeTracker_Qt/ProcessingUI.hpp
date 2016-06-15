#pragma once
#include <QWidget>
#include "ui_processingui.h"

namespace gt
{
	class ProcessingUI : public QWidget
	{
		Q_OBJECT

	public:
		ProcessingUI(QWidget* parent = Q_NULLPTR);
		~ProcessingUI();

		signals:
		void closing();
	public slots:
		void show();
		void close();
		void stateChanged(int);
		void changeProgess(double) const;

	protected:
		void closeEvent(QCloseEvent* event) override;
	private:
		Ui::ProcessingUI ui;
	};
}
