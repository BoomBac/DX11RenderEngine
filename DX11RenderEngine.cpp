#include "DX11RenderEngine.h"
#include <QLabel>
#include "vector2D.h"
#include "RenderFrame.h"
#include <QTimer>

DX11RenderEngine::DX11RenderEngine(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setFixedSize(1280, 720);
    QLabel *lb_pos = new QLabel(QString("MousePos:"), this);
    ui.statusBar->addWidget(lb_pos);
    QLabel* lb_state = new QLabel(QString("MouseState:"), this);
    ui.statusBar->addWidget(lb_state);

    connect(ui.renderView, &RenderViewport::MouseMoved, [=](QString pos) {
        lb_pos->setText(QString("MousePos:") + pos);
        });
    connect(ui.renderView, &RenderViewport::MousePressed, [=](QString state) {
        lb_state->setText(QString("MouseState:") + state);
        });
    connect(ui.renderView, &RenderViewport::MouseReleased, [=](QString state) {
        lb_state->setText(QString("MouseState:") + state);
        });

    //初始化renderViewport，传入hWnd初始化dx
    hWnd = (HWND)ui.renderView->winId();
   renderframe = new RenderFrame(hWnd);
    QTimer *timer = new QTimer(this);
    timer->start(12);  //0.5s
    connect(timer, &QTimer::timeout, [=]()
    {
            const_cast<RenderFrame*>(renderframe)->EndFrame();
    });  //每隔0.5s发出一
}

DX11RenderEngine::~DX11RenderEngine()
{
    if (renderframe != nullptr)
        delete renderframe;
}
