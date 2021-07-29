#pragma once
#include "Timer.h"
#include <QDebug>



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
};


