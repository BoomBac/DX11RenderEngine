#ifndef DX11ENGINE_QTFRAME_RENDERVIEWPORT_H
#define DX11ENGINE_QTFRAME_RENDERVIEWPORT_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include <QWidget>
#include "ui_RenderViewport.h"

#include "vector3D.h"
#include "Public/Render/Graphics.h"
#include "../Tool/ObserverInterface.h"


class RenderViewport : public QWidget,public ObserverInterface
{
	Q_OBJECT
public:
	RenderViewport(QWidget *parent = Q_NULLPTR);
	~RenderViewport();
	virtual void keyPressEvent(QKeyEvent* event);  //键盘按下事件
	virtual void keyReleaseEvent(QKeyEvent* event);  //键盘按下事件
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	//在主窗口中初始化dx
	void InitialViewport();
	//每帧更新
	void UpdateViewport();
	//Graphics和Window的中转函数
	//设置当前选中物体的变换，flag 0:translate,1:rotate,2:scale
	void SetSelectedObjectTransform(const CusMath::vector3d& pos, const char& flag);
	void SetbgColor(float color[4]);
	void SetCoordinateType(bool is_world);
	// 0 box; 1 plane
	void AddSceneObject(char type);
	void SetSelectObject(int index,double tranf_info[9]);
	void DeleteSceneObject(const int& index);

	Graphics* graphicsIns = nullptr;
	bool object_rotation_changed_ = true;

signals:
	void MouseMoved(QString& pos);
	void MousePressed(QString& state);
	void MouseReleased(QString& state);
	//切换坐标轴类型时，物体空间信息框更新 
	//flag 0 1 2 世界位移，旋转，缩放 
	//3 4 局部位移，旋转
	void ActorTransformChange(const CusMath::vector3d& new_tranf, char flag);

	void SceneObjectAdd(const char* object_name);

public slots:

private:
	DISALLOW_COPY_AND_ASSIGN(RenderViewport)
	Ui::RenderViewport ui;
	QString mouPos;
	QString moubtState;
	//鼠标位置,每当鼠标移动或者按下时赋值为当前鼠标位置
	int posX = 0;
	int posY = 0;
	virtual void OnOutlineChanged(bool is_add) override;
};

#endif // DX11ENGINE_QTFRAME_RENDERVIEWPORT_H