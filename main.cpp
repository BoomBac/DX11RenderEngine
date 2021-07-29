#include "DX11RenderEngine.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DX11RenderEngine w;
    w.show();
    return a.exec();
}
