#include <Public/Render/Graphics.h>
#ifndef DX11ENGINE_RENDER_RESMANAGE_RESMANAGE_H
#define DX11ENGINE_RENDER_RESMANAGE_RESMANAGE_H


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif 

class ResManage
{
public:
    static void InitResManage(Graphics* gfx);
protected:
    ResManage() {};
    static Graphics* gfx_;
private:
    DISALLOW_COPY_AND_ASSIGN(ResManage)
};

#endif//DX11ENGINE_RENDER_RESMANAGE_RESMANAGE_H