#include "DX11RenderEngine.h"
#include "Global.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //��ʼ��ȫ�ֵ���
    Global::getInstance();
    DX11RenderEngine w;
    w.show();
    return a.exec();
}
