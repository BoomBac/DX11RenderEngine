#pragma once
#include <QDebug>
#include "Tool/Timer.h"
#include <Public/Render/Graphics.h>


class Global
{
private:
    Global() {};
	static Global* m_instance;
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (m_instance)
            {
                qDebug() << "deleteeeeeeeeeeeeeeeeeeeeeee";
                delete(m_instance);
            }
        }
    };
    static CGarbo gc;
public:
	static Global* getInstance();

public:
    RE::Timer gTimer;
    ID3D11Device* G_Device = nullptr;
    ID3D11DeviceContext* G_Context = nullptr;
    Graphics* G_Gfx = nullptr;
};


