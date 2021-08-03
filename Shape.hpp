#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include <QDebug>
#include "Utili.h"

template<class T>  //每个模板子类静态成员不共享
class Shape :
    public Drawable
{

public:
    bool isInitialzed() const;
    void AddStaticBind(std::unique_ptr<Bindable> bind);
    void AddStaticIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
    //更新常量缓冲区中的世界变换

    virtual void SetActorLocation(const CusMath::vector3d& t)  override;
	virtual void SetActorRotation(const CusMath::vector3d& r)override;
	virtual void SetActorScale(const CusMath::vector3d& s) override;
private:
    static std::vector<std::unique_ptr<Bindable>> StaticBinds;
private:
    virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const override
    {
        return StaticBinds;
    }
    //传入变换矩阵
    void Update(const DirectX::XMMATRIX& transf);
protected:
    void SetIndexbufferFromSBinds();
};

template<class T>
void Shape<T>::SetActorScale(const CusMath::vector3d& s)
{
    Drawable::SetActorScale(s);
    DonedTransforms[0] = DirectX::XMMatrixScaling(s.x, s.y, s.z);
    Update(DonedTransforms[2] *DonedTransforms[1] *DonedTransforms[0]);
}

template<class T>
void Shape<T>::SetActorRotation(const CusMath::vector3d& r)
{
    Drawable::SetActorRotation({ DegToRad(r.x), DegToRad(r.y), DegToRad(r.z)});
    DonedTransforms[1] = DirectX::XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    Update(DonedTransforms[2] * DonedTransforms[1] * DonedTransforms[0]);
}

template<class T>
void Shape<T>::SetActorLocation(const CusMath::vector3d& t)
{
    Drawable::SetActorLocation(t);
    Update(DirectX::XMMatrixTranslation(t.x, t.y, t.z));
}

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

