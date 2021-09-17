#include "DX11RenderEngine.h"
#include <QLabel>
#include <QTimer>
#include "Global.h"
#include "QScrollBar"
#include "QPushButton"
#include <QColorDialog>


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
    connect(ui.hSBar_CameraRX, &QScrollBar::valueChanged, [=](const float& angle) {
        ui.renderView->OnCameraSlideBarChanged(angle, (float)ui.hSBar_CameraRY->value(), (float)ui.hSBar_CameraRZ->value());
        });
	connect(ui.hSBar_CameraRY, &QScrollBar::valueChanged, [=](const float& angle) {
		ui.renderView->OnCameraSlideBarChanged((float)ui.hSBar_CameraRX->value(), angle, (float)ui.hSBar_CameraRZ->value());
		});
	connect(ui.hSBar_CameraRZ, &QScrollBar::valueChanged, [=](const float& angle) {
		ui.renderView->OnCameraSlideBarChanged((float)ui.hSBar_CameraRX->value(), (float)ui.hSBar_CameraRY->value(),angle );
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
    connect(ui.dSB_MSX,&QDoubleSpinBox::valueChanged, [=](const double& val){
        ui.renderView->SetSelectedObjectTransform(
                { (float)val, (float)ui.dSB_MSY->value(),(float)ui.dSB_MSZ->value() }, char(2));
        });
	connect(ui.dSB_MSY, &QDoubleSpinBox::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform(
			{ (float)ui.dSB_MSX->value(),(float)val, (float)ui.dSB_MSZ->value() }, char(2));
		});
	connect(ui.dSB_MSZ, &QDoubleSpinBox::valueChanged, [=](const double& val) {
		ui.renderView->SetSelectedObjectTransform(
			{ (float)ui.dSB_MSX->value(),(float)ui.dSB_MSY->value(),(float)val,}, char(2));
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
    timer->start(24);  //0.5s
    connect(timer, &QTimer::timeout, [=]()
    {
       lb_time->setText(QString::number(Global::getInstance()->gTimer.Peek(), 'f', 2));
       //lb_time->setText(QString("%1").arg(Global::getInstance()->gTimer.Peek()));
    });  //每隔0.5s发出一
}

DX11RenderEngine::~DX11RenderEngine()
{
}

//似乎每帧调用，暂时用它模仿tick
void DX11RenderEngine::paintEvent(QPaintEvent* e)
{
    ui.renderView->UpdateViewport();
}


