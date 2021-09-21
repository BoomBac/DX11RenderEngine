#pragma once

#include <QDebug>
#include "Public\Tool\Utili.h"

template<class T>  //每个模板子类静态成员不共享
class Shape :
	public Drawable
{

public:
	bool isInitialzed() const;
	void AddStaticBind(std::unique_ptr<Bindable> bind);
	void AddStaticIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
	virtual void OnCameraTransChanged() override;	
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
	virtual void Update(const DirectX::XMMATRIX& transf=DirectX::XMMatrixIdentity()) override;
protected:
	void SetIndexbufferFromSBinds();
};

template<class T>
void Shape<T>::Update(const DirectX::XMMATRIX& transf/*=DirectX::XMMatrixIdentity()*/)
{
	//先平移。如果先缩放，那么平移的轴距就会被缩放改变。如果先旋转，那么绕三个轴旋转就会变成绕世界原点的旋转
	transform.mWorld = transf * DonedTransforms[1] * DonedTransforms[2] * DonedTransforms[0] *  view * projection;
}

template<class T>
void Shape<T>::OnCameraTransChanged()
{
	Update();
}

template<class T>
void Shape<T>::SetActorScale(const CusMath::vector3d& s)
{
	Drawable::SetActorScale(s);
	DonedTransforms[2] = DirectX::XMMatrixScaling(s.x, s.y, s.z);
	Update();
}

template<class T>
void Shape<T>::SetActorRotation(const CusMath::vector3d& r)
{
	Drawable::SetActorRotation({ DegToRad(r.x), DegToRad(r.y), DegToRad(r.z)});
	DonedTransforms[1] = DirectX::XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	Update();
}

template<class T>
void Shape<T>::SetActorLocation(const CusMath::vector3d& t)
{
	Drawable::SetActorLocation(t);
	DonedTransforms[0] = DirectX::XMMatrixTranslation(t.x, t.y, t.z);
	Update();
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

