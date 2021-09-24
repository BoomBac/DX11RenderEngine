#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include <QPushButton>
#include <QColorDialog>

#include "Public/Global.h"
#include "Public/QtFrame/DX11RenderEngine.h"
#include "vector3D.h"
#include "QComboBox"
#include "Public/QtFrame/RenderViewport.h"
#include "Public/Render/Drawable/Drawable.h"


DX11RenderEngine::DX11RenderEngine(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setFixedSize(1280, 720);

    //创建状态栏相关
    QLabel *lb_pos = new QLabel(QString("MousePos:"), this);
    ui.statusBar->addWidget(lb_pos);
    QLabel* lb_state = new QLabel(QString("MouseState:"), this);
    ui.statusBar->addWidget(lb_state);
    QLabel* lb_time = new QLabel(QString("CurrentTime:"), this);
    ui.statusBar->addWidget(lb_time);


    connect(ui.renderView, &RenderViewport::MouseMoved, [=](QString pos) {
        lb_pos->setText(QString("MousePos:") + pos);
        });
    connect(ui.renderView, &RenderViewport::MousePressed, [=](QString state) {
        lb_state->setText(QString("MouseState:") + state);
        });
    connect(ui.renderView, &RenderViewport::MouseReleased, [=](QString state) {
        lb_state->setText(QString("MouseState:") + state);
        });
    connect(ui.renderView, &RenderViewport::ActorTransformChange, [=](const CusMath::vector3d& tranf, char flag) {
        ui.dSB_MTX->blockSignals(true);
        ui.dSB_MTY->blockSignals(true);
        ui.dSB_MTZ->blockSignals(true);
        ui.dSB_MTX->setValue(static_cast<double>(tranf.x));
        ui.dSB_MTY->setValue(static_cast<double>(tranf.y));
        ui.dSB_MTZ->setValue(static_cast<double>(tranf.z));
		ui.dSB_MTX->blockSignals(false);
		ui.dSB_MTY->blockSignals(false);
		ui.dSB_MTZ->blockSignals(false);
        });
    //更改坐标轴类型
    connect(ui.CB_Coord, &QComboBox::currentIndexChanged, [=](int index){
        if (index == 0)
            ui.renderView->SetCoordinateType(true);
        else
            ui.renderView->SetCoordinateType(false);
        });

    //test
    connect(ui.bt_test, &QPushButton::clicked, [=]() {
       });

    //模型变换绑定
    connect(ui.dSB_MTX, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({ (float)val,(float)ui.dSB_MTY->value(),
            (float)ui.dSB_MTZ->value() }, char(0));
        });
    connect(ui.dSB_MTY, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({ (float)ui.dSB_MTX->value(),(float)val,
            (float)ui.dSB_MTZ->value() }, char(0));
        });
    connect(ui.dSB_MTZ, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({ 
            (float)ui.dSB_MTX->value(),(float)ui.dSB_MTY->value(),(float)val }, char(0));
        });
    connect(ui.dSB_MRX, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({
           (float)val,(float)ui.dSB_MRY->value(),(float)ui.dSB_MRZ->value() }, char(1));
        });
    connect(ui.dSB_MRY, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({
           (float)ui.dSB_MRX->value(),(float)val,(float)ui.dSB_MRZ->value() }, char(1));
        });
    connect(ui.dSB_MRZ, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform({
           (float)ui.dSB_MRX->value(),(float)ui.dSB_MRY->value(),(float)val }, char(1));
        });
    connect(ui.dSB_MSX, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform(
            { (float)val, (float)ui.dSB_MSY->value(),(float)ui.dSB_MSZ->value() }, char(2));
        });
    connect(ui.dSB_MSY, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform(
            { (float)ui.dSB_MSX->value(),(float)val, (float)ui.dSB_MSZ->value() }, char(2));
        });
    connect(ui.dSB_MSZ, &QDoubleSpinBox::valueChanged, [=](const double& val) {
        ui.renderView->SetSelectedObjectTransform(
            { (float)ui.dSB_MSX->value(),(float)ui.dSB_MSY->value(),(float)val, }, char(2));
        });
    //选择背景颜色对话框
    connect(ui.bt_ChooseBC, &QPushButton::clicked, [=]() {
        static float bgc[4] = { 0.f,0.f,0.f,1.f };
        QColor color = QColorDialog::getColor(Qt::black, this, "choose color");
        bgc[0] = color.redF();
        bgc[1] = color.greenF();
        bgc[2] = color.blueF();
        ui.renderView->SetbgColor(bgc);
        });

    //初始化dx渲染视口
    ui.renderView->InitialViewport();

    //设置定时器，此处模拟tick，后续可能更改
    QTimer *timer = new QTimer(this);
    timer->start(16.7);  // fps = num/1000 16.7的话近似于60fps
    connect(timer, &QTimer::timeout, [=]()
    {
        //每次修改界面内容都会调用paintEvent()
       lb_time->setText(QString::number(Global::getInstance()->gTimer.Peek(), 'f', 2));
    }); 
}

DX11RenderEngine::~DX11RenderEngine()
{

}

void DX11RenderEngine::paintEvent(QPaintEvent* e)
{
    ui.renderView->UpdateViewport();
}


