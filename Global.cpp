#include "Global.h"

Global* Global::m_instance = nullptr;


Global* Global::getInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = new Global();
        return m_instance;
    }
    return m_instance;
}
