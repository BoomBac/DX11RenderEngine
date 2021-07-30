#include "DX11RenderEngine.h"
#include <QLabel>
#include <QTimer>
#include "Global.h"



DX11RenderEngine::DX11RenderEngine(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setFixedSize(1280, 720);

    //����״̬�����
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
    //��ʼ��dx��Ⱦ�ӿ�
    ui.renderView->InitialViewport();

    //���ö�ʱ�����˴�ģ��tick���������ܸ���
    QTimer *timer = new QTimer(this);
    timer->start(12);  //0.5s
    connect(timer, &QTimer::timeout, [=]()
    {
       lb_time->setText(QString::number(Global::getInstance()->gTimer.Peek(), 'f', 2));
       //lb_time->setText(QString("%1").arg(Global::getInstance()->gTimer.Peek()));
    });  //ÿ��0.5s����һ
}

DX11RenderEngine::~DX11RenderEngine()
{
}

//�ƺ�ÿ֡���ã���ʱ����ģ��tick
void DX11RenderEngine::paintEvent(QPaintEvent* e)
{
    ui.renderView->UpdateViewport();
}


