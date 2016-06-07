#pragma once
#include <QLabel>

//like http://mangoprojects.info/c/qtc-drawing-a-line-or-rectangle-dynamically/
class EyeTemplateSelect : public QLabel {
	Q_OBJECT
public:
	EyeTemplateSelect(QWidget * parent = Q_NULLPTR);
	~EyeTemplateSelect();

signals:
	void mouseLeftClicked();
	void mouseLeftReleased(const QRect&);
	void mouseRightClicked();

public slots:
	void reset();
	void imageAvailable();

protected:
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

private:
	bool m_ImageAvailable = false;
	bool m_MouseDown = false;
	bool m_DrawStarted = false;
	QPainter m_Painter;
	QPixmap m_Pixmap;
	QRect m_Rect;
};
