#pragma once
#include <QWidget>
#include "ui_processingui.h"
namespace gt
{
	
	class ProcessingUI : public QWidget {
		Q_OBJECT

	public:
		ProcessingUI(QWidget * parent = Q_NULLPTR);
		~ProcessingUI();

	public slots:
		void show();
		void stateChanged(int);
		void changeProgess(double) const;
	private:
		Ui::ProcessingUI ui;
	};

}