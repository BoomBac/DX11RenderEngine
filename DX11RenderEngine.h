#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DX11RenderEngine.h"

class RenderFrame;
class DX11RenderEngine : public QMainWindow
{
    Q_OBJECT

public:
    DX11RenderEngine(QWidget *parent = Q_NULLPTR);
    ~DX11RenderEngine();
    HWND hWnd;
private:
    Ui::DX11RenderEngineClass ui;
    RenderFrame* renderframe = nullptr;
};
