#include <stdafx.h>
#include "startui.hpp"
using namespace gt;

StartUI::StartUI(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	QObject::connect(ui.aboutButton, SIGNAL(clicked()), this, SLOT(showAbout()));
	QObject::connect(ui.recordDataCheckBox, SIGNAL(stateChanged(int)), this, SLOT(recordDataChanged(int)));
	QObject::connect(ui.startButton, SIGNAL(clicked()), this, SLOT(startApplication()));

	m_FileDialog = new QFileDialog(this);
	m_FileDialog->setFileMode(QFileDialog::DirectoryOnly);
	m_FileDialog->setOption(QFileDialog::ShowDirsOnly);
	m_FileDialog->setViewMode(QFileDialog::List);
	m_FileDialog->setAcceptMode(QFileDialog::AcceptOpen);
	m_FileDialog->setModal(true);
	m_FileDialog->setDirectory(DataTrackingSystem::GetInstance()->DefaultStorageFolder.c_str());

	QObject::connect(ui.folderBrowserButton, SIGNAL(clicked()), m_FileDialog, SLOT(show()));
	QObject::connect(m_FileDialog, SIGNAL(currentChanged(const QString&)), this, SLOT(pathChanged(const QString&)));

	ui.folderLabel->setText(DataTrackingSystem::GetInstance()->DefaultStorageFolder.c_str());
	ui.recordDataCheckBox->setChecked(Configuration::GetInstance()->GetRecordData());
}

StartUI::~StartUI()
{
}

void StartUI::show()
{
	QWidget::show();
	GazeTrackerManager::GetInstance()->SetActiveState(GazeTrackerState::Start_UI);
}

void StartUI::startApplication()
{
	UISystem::GetInstance()->GetConfigurationUI()->show();
	if (Configuration::GetInstance()->GetRecordData())
		DataTrackingSystem::GetInstance()->OpenFile();
	//GazeTrackerManager::GetInstance()->detect();
}

void StartUI::showAbout()
{
	QMessageBox::information(this, tr("GazeTracker v1.0"), tr("This application was made by Michael Pichler and Bernhard Rieder."));
}

void StartUI::recordDataChanged(int state) const
{
	bool res = state == Qt::Checked;
	Configuration::GetInstance()->SetRecordData(res);
	ui.folderBrowserButton->setEnabled(res);
	ui.folderLabel->setEnabled(res);
}

void StartUI::pathChanged(const QString& path)
{
	DataTrackingSystem::GetInstance()->SetStoragePath(path.toStdString());
	ui.folderLabel->setText(path);
}

