#include <stdafx.h>
#include "cornerconfigurationui.hpp"
using namespace gt;

CornerConfigurationUI::CornerConfigurationUI(QWidget* parent) : QWidget(parent), m_setCorners(0)
{
	ui.setupUi(this);

	QObject::connect(ui.leftTopButton, SIGNAL(toggled(bool)), this, SLOT(saveTopLeft(bool)));
	QObject::connect(ui.leftBottomButton, SIGNAL(toggled(bool)), this, SLOT(saveBottomLeft(bool)));
	QObject::connect(ui.leftMiddleButton, SIGNAL(toggled(bool)), this, SLOT(saveMiddleLeft(bool)));
	QObject::connect(ui.middleTopButton, SIGNAL(toggled(bool)), this, SLOT(saveTopMiddle(bool)));
	QObject::connect(ui.middleButton, SIGNAL(toggled(bool)), this, SLOT(saveMiddleMiddle(bool)));
	QObject::connect(ui.middleBottomButton, SIGNAL(toggled(bool)), this, SLOT(saveBottomMiddle(bool)));
	QObject::connect(ui.rightTopButton, SIGNAL(toggled(bool)), this, SLOT(saveTopRight(bool)));
	QObject::connect(ui.rightMiddleButton, SIGNAL(toggled(bool)), this, SLOT(saveMiddleRight(bool)));
	QObject::connect(ui.rightBottomButton, SIGNAL(toggled(bool)), this, SLOT(saveBottomRight(bool)));

	//without middle button
	m_CornerButtons.push_back(ui.leftTopButton);
	m_CornerButtons.push_back(ui.leftBottomButton);
	m_CornerButtons.push_back(ui.leftMiddleButton);
	m_CornerButtons.push_back(ui.middleTopButton);
	m_CornerButtons.push_back(ui.middleBottomButton);
	m_CornerButtons.push_back(ui.rightTopButton);
	m_CornerButtons.push_back(ui.rightMiddleButton);
	m_CornerButtons.push_back(ui.rightBottomButton);

	resetCorners();
}

CornerConfigurationUI::~CornerConfigurationUI()
{
}

void CornerConfigurationUI::show()
{
	resetCorners();
	QWidget::show();
	GazeTrackerManager::GetInstance()->Start();
	showFullScreen();
	QMessageBox::information(this, tr("Important Note!"), tr("Every button represents a corner of your screen. Please focus your eyes on one button per time and click on it! Start with the center!"), QMessageBox::Ok);
}

void CornerConfigurationUI::close()
{
	GazeTrackerManager::GetInstance()->Stop();
	if (allCornersSet())
		emit configurationSuccess();
	QWidget::close();
}

void CornerConfigurationUI::saveTopLeft(bool var)
{
	saveCurrentCornerAs(ui.leftTopButton, var, Configuration::Corners::TopLeft);
}

void CornerConfigurationUI::saveTopMiddle(bool var)
{
	saveCurrentCornerAs(ui.middleTopButton, var, Configuration::Corners::Top);
}

void CornerConfigurationUI::saveTopRight(bool var)
{
	saveCurrentCornerAs(ui.rightTopButton, var, Configuration::Corners::TopRight);
}

void CornerConfigurationUI::saveMiddleLeft(bool var)
{
	saveCurrentCornerAs(ui.leftMiddleButton, var, Configuration::Corners::Left);
}

void CornerConfigurationUI::saveMiddleMiddle(bool var)
{
	saveCurrentCornerAs(ui.middleButton, var, Configuration::Corners::Center);

	if (var)
		for (auto& btn : m_CornerButtons)
			btn->setEnabled(var);
	else
		resetCornerButtons();
}

void CornerConfigurationUI::saveMiddleRight(bool var)
{
	saveCurrentCornerAs(ui.rightMiddleButton, var, Configuration::Corners::Right);
}

void CornerConfigurationUI::saveBottomLeft(bool var)
{
	saveCurrentCornerAs(ui.leftBottomButton, var, Configuration::Corners::BottomLeft);
}

void CornerConfigurationUI::saveBottomMiddle(bool var)
{
	saveCurrentCornerAs(ui.middleBottomButton, var, Configuration::Corners::Bottom);
}

void CornerConfigurationUI::saveBottomRight(bool var)
{
	saveCurrentCornerAs(ui.rightBottomButton, var, Configuration::Corners::BottomRight);
}

void CornerConfigurationUI::saveCurrentCornerAs(QPushButton* btn, bool var, Configuration::Corners corner)
{
	if (var)
	{
		QtHelper::ChangeBackgroundColor(*btn, "green");
		m_setCorners += static_cast<int>(corner);
		auto pos = GazeTrackerManager::GetInstance()->GetLastDetectedIrisesPositions();
		Configuration::GetInstance()->SetCorner(pos.left, corner, Configuration::Iris::Left);
		Configuration::GetInstance()->SetCorner(pos.right, corner, Configuration::Iris::Right);
		if(allCornersSet())
		{
			int ret = QMessageBox::information(this, tr("Corners set!"), tr("All corners set! Apply?"), QMessageBox::Yes | QMessageBox::No);
			if (ret == QMessageBox::Yes)
				close();
			else
				resetCorners();
		}
	}
	else
	{
		QtHelper::ChangeBackgroundColor(*btn, "red");
		m_setCorners -= static_cast<int>(corner);
	}
}

bool CornerConfigurationUI::allCornersSet() const
{
	return m_setCorners == (
		static_cast<int>(Configuration::Corners::TopLeft) +
		static_cast<int>(Configuration::Corners::Top) +
		static_cast<int>(Configuration::Corners::TopRight) +
		static_cast<int>(Configuration::Corners::Left) +
		static_cast<int>(Configuration::Corners::Center) +
		static_cast<int>(Configuration::Corners::Right) +
		static_cast<int>(Configuration::Corners::BottomLeft) +
		static_cast<int>(Configuration::Corners::Bottom) +
		static_cast<int>(Configuration::Corners::BottomRight));
}

void CornerConfigurationUI::resetCorners() const
{
	ui.middleButton->setChecked(false);
	QtHelper::ChangeBackgroundColor(*ui.middleButton, "red");
	resetCornerButtons();
}

void CornerConfigurationUI::resetCornerButtons() const
{
	for (auto& btn : m_CornerButtons)
	{
		QtHelper::ChangeBackgroundColor(*btn, "red");
		btn->setChecked(false);
		btn->setEnabled(false);
	}
}
