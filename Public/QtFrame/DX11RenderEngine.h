#ifndef DX11ENGINE_QTFRAME_DX11RENDERENGINE_H
#define DX11ENGINE_QTFRAME_DX11RENDERENGINE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include <QtWidgets/QMainWindow>
#include <QCheckBox>

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

    void InitLightDetail();
    void InitTransformDetail();
    void InitRenderDetail();
    //light property
    QWidget* p_light_detail_;
    QTableWidget* p_light_property_;
    QPushButton* p_light_color_;
    ProgressLine* p_light_intensity_;
    ProgressLine* p_light_radius_;
    ProgressLine* p_light_i_angle_;
    ProgressLine* p_light_o_angle_;
    void AdjustLightProperty(ELightType pre_type, ELightType next_type);

    //render property
    QCheckBox* p_cb_visiblity_;
    QCheckBox* p_cb_shadow_;
    ProgressLine* p_shadow_light_size_;
    ProgressLine* p_shadow_light_near_;
    ProgressLine* p_shadow_light_far_;
    ProgressLine* p_shadow_bias_;
    void AdjustRenderProperty();
};
#endif