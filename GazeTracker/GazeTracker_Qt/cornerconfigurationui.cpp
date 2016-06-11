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
}

CornerConfigurationUI::~CornerConfigurationUI()
{
}

void CornerConfigurationUI::show()
{
	QWidget::show();
	GazeTrackerManager::GetInstance()->Start();
	showFullScreen();
	QMessageBox::information(this, tr("Important Note!"), tr("Every button represents a corner of your screen. Please focus your eyes on one button per time and click on it!"), QMessageBox::Ok);
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
	saveCurrentCornerAs(var, Configuration::Corners::TopLeft);
}

void CornerConfigurationUI::saveTopMiddle(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::Top);
}

void CornerConfigurationUI::saveTopRight(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::TopRight);
}

void CornerConfigurationUI::saveMiddleLeft(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::Left);
}

void CornerConfigurationUI::saveMiddleMiddle(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::Center);
}

void CornerConfigurationUI::saveMiddleRight(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::Right);
}

void CornerConfigurationUI::saveBottomLeft(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::BottomLeft);
}

void CornerConfigurationUI::saveBottomMiddle(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::Bottom);
}

void CornerConfigurationUI::saveBottomRight(bool var)
{
	saveCurrentCornerAs(var, Configuration::Corners::BottomRight);
}

void CornerConfigurationUI::saveCurrentCornerAs(bool var, Configuration::Corners corner)
{
	if (var)
	{
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
	ui.leftTopButton->setChecked(false);
	ui.leftBottomButton->setChecked(false);
	ui.leftMiddleButton->setChecked(false);
	ui.middleTopButton->setChecked(false);
	ui.middleButton->setChecked(false);
	ui.middleBottomButton->setChecked(false);
	ui.rightTopButton->setChecked(false);
	ui.rightMiddleButton->setChecked(false);
	ui.rightBottomButton->setChecked(false);
}
