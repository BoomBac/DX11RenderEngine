#include "DX11RenderEngine.h"
#include "Global.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //初始化全局单例
    Global::getInstance();
    DX11RenderEngine w;
    w.show();
    return a.exec();
}
