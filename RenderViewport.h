#pragma once

#include <QWidget>
#include "ui_RenderViewport.h"

class Graphics;

class RenderViewport : public QWidget
{
	Q_OBJECT

public:
	RenderViewport(QWidget *parent = Q_NULLPTR);
	~RenderViewport();

	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	//在主窗口中初始化dx
	void InitialViewport();
	//每帧更新
	void UpdateViewport();

	Graphics* renderframe = nullptr;
signals:
	void MouseMoved(QString& pos);
	void MousePressed(QString& state);
	void MouseReleased(QString& state);
private:
	Ui::RenderViewport ui;
	QString mouPos;
	QString moubtState;
};
