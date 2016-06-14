#include "stdafx.h"
#include "ProcessingUI.hpp"
#include "GazeDataProcessing.hpp"

using namespace gt;

ProcessingUI::ProcessingUI(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);
}

ProcessingUI::~ProcessingUI() {
	
}

void ProcessingUI::show()
{
	QWidget::show();
	changeProgess(0);
}

void ProcessingUI::changeProgess(double percent) const
{
	ui.progressBar->setValue(percent);
}

void ProcessingUI::stateChanged(int x)
{
	switch(static_cast<GazeDataProcessingState>(x))
	{
		case GazeDataProcessingState::OpenFile: 
			ui.label->setText("Open File..."); 
			break;
		case GazeDataProcessingState::ReadData: 
			ui.label->setText("Read Data..."); 
			break;
		case GazeDataProcessingState::CreateVideo: 
			ui.label->setText("Create Video..."); break;
		case GazeDataProcessingState::FinishedWithSuccess: 
			QMessageBox::warning(this, tr("Information"), tr("Everything gone right! File saved!"), QMessageBox::Ok);
			close();
			break;
		case GazeDataProcessingState::FinishedWithError: 
			QMessageBox::warning(this, tr("WARNING"), tr("Something went wrong!"), QMessageBox::Ok); 
			close(); 
			break;
		default: break;
	}
}
