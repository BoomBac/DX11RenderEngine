#ifndef DX11ENGINE_QTFRAME_DX11RENDERENGINE_H
#define DX11ENGINE_QTFRAME_DX11RENDERENGINE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include <QtWidgets/QMainWindow>

#include "ExternalWidgets/progressline.h"

#include "ui_DX11RenderEngine.h"



class DX11RenderEngine : public QMainWindow
{
    Q_OBJECT
public:
    DX11RenderEngine(QWidget *parent = Q_NULLPTR);
    ~DX11RenderEngine();
private:
    DISALLOW_COPY_AND_ASSIGN(DX11RenderEngine)
    void paintEvent(QPaintEvent* e);
    virtual void keyPressEvent(QKeyEvent* event) override;
    Ui::DX11RenderEngineClass ui;
    //记录物体变换框数据
    float transform_info_[6]{ 0.f,0.f,0.,0.f,0.f,0.f};
    void ChangeCoordinateType(int index);
    void OnOutlineItemChanged(int row);
    //transform progress
    ProgressLine* p_location_x_;
    ProgressLine* p_location_y_;
    ProgressLine* p_location_z_;
	ProgressLine* p_rotation_x_;
	ProgressLine* p_rotation_y_;
	ProgressLine* p_rotation_z_;
	ProgressLine* p_scale_x_;
	ProgressLine* p_scale_y_;
	ProgressLine* p_scale_z_;
};

#endif