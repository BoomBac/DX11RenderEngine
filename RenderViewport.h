#pragma once

#include <QWidget>
#include "ui_RenderViewport.h"

class RenderViewport : public QWidget
{
	Q_OBJECT

public:
	RenderViewport(QWidget *parent = Q_NULLPTR);
	~RenderViewport();

	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
signals:
	void MouseMoved(QString& pos);
	void MousePressed(QString& state);
	void MouseReleased(QString& state);
private:
	Ui::RenderViewport ui;
	QString mouPos;
	QString moubtState;
};
