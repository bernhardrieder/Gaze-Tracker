#include <stdafx.h>
#include "EyeTemplateSelect.hpp"
using namespace gt;

EyeTemplateSelect::EyeTemplateSelect(QWidget* parent) : QLabel(parent)
{
	setMouseTracking(true);
	setAlignment(Qt::AlignLeft);
	setAlignment(Qt::AlignTop);
}

EyeTemplateSelect::~EyeTemplateSelect()
{
}

void EyeTemplateSelect::reset()
{
	m_Rect = QRect();
}

void EyeTemplateSelect::imageAvailable()
{
	m_ImageAvailable = true;
}

void EyeTemplateSelect::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_ImageAvailable) return;
	if (m_MouseDown)
	{
		if (event->type() == QEvent::MouseMove)
			m_Rect.setBottomRight(event->pos());

		update();
	}
}

void EyeTemplateSelect::mousePressEvent(QMouseEvent* event)
{
	if (!m_ImageAvailable) return;
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		emit mouseLeftClicked();
		m_MouseDown = true;
		m_Rect.setTopLeft(event->pos());
		m_Rect.setBottomRight(event->pos());
	}
	else if (event->button() == Qt::MouseButton::RightButton)
	{
		emit mouseRightClicked();
		reset();
		update();
	}
}

void EyeTemplateSelect::mouseReleaseEvent(QMouseEvent* event)
{
	if (!m_ImageAvailable) return;
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		emit mouseLeftReleased(m_Rect);
		m_MouseDown = false;
		update();
	}
}

void EyeTemplateSelect::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);
	m_Painter.begin(this);
	m_Painter.setPen(Qt::red);
	if (m_MouseDown && !pixmap()->isNull())
	{
		m_Painter.drawPixmap(0, 0, m_Pixmap);
		m_Painter.drawRect(m_Rect);

		m_DrawStarted = true;
	}
	else if (m_DrawStarted && !pixmap()->isNull())
	{
		m_Painter.drawPixmap(0, 0, m_Pixmap);
		m_Painter.drawRect(m_Rect);
	}
	m_Painter.end();
}

