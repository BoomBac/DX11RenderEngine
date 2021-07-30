#include "DX11RenderEngine.h"
#include <QLabel>
#include <QTimer>
#include "Global.h"



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
    //初始化dx渲染视口
    ui.renderView->InitialViewport();

    //设置定时器，此处模拟tick，后续可能更改
    QTimer *timer = new QTimer(this);
    timer->start(12);  //0.5s
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


