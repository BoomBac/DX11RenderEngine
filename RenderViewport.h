#pragma once

#include <QWidget>
#include "ui_RenderViewport.h"
#include "vector3D.h"

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
	Graphics* graphicsIns = nullptr;

	//Graphics和Window的中转函数
	void SetbgColor(float color[4]);
	//设置当前选中物体的变换，flag 0:translate,1:rotate,2:scale
	void SetSelectedObjectTransform(const CusMath::vector3d& pos, const char& flag);
signals:
	void MouseMoved(QString& pos);
	void MousePressed(QString& state);
	void MouseReleased(QString& state);
public slots:
	//Graphics和Window的中转函数
	void OnCameraSlideBarChanged(const float& x, const float& y, const float& z);
private:
	Ui::RenderViewport ui;
	QString mouPos;
	QString moubtState;
};
