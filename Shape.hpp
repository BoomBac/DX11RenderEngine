#pragma once
#include "Drawable.h"
#include "Bindable.h"

template<class T>  //每个模板子类静态成员不共享
class Shape :
    public Drawable
{

public:
    bool isInitialzed() const;
    void AddStaticBind(std::unique_ptr<Bindable> bind);
    void AddStaticIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
    //更新常量缓冲区中的世界变换
    void Update(const DirectX::XMMATRIX& transf);
private:
    static std::vector<std::unique_ptr<Bindable>> StaticBinds;
private:
    virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const override
    {
        return StaticBinds;
    }
protected:
    void SetIndexbufferFromSBinds();
};

template<class T>
void Shape<T>::Update(const DirectX::XMMATRIX& transf)
{
	transform.mWorld = transf*view*projection;
}

template<class T>
void Shape<T>::SetIndexbufferFromSBinds()
{
    for (auto& i : StaticBinds)
    {
        if (i->GetType() == EBindableType::IndexBuffer)
        {
            indexbuffer = dynamic_cast<IndexBuffer*>(i.get());
        }
    }
}

template<class T>
std::vector<std::unique_ptr<Bindable>> Shape<T>::StaticBinds;

template<class T>
bool Shape<T>::isInitialzed() const
{
	return !StaticBinds.empty();
}
template<class T>
void Shape<T>::AddStaticBind(std::unique_ptr<Bindable> bind)
{
	StaticBinds.push_back(std::move(bind));
}
template<class T>
void Shape<T>::AddStaticIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx)
{
    indexbuffer = ibf.get();
    StaticBinds.push_back(std::move(ibf));
}

