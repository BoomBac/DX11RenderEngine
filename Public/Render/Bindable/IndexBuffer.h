#pragma once
#include "Bindable.h"



class IndexBuffer : public Bindable
{
public:
	IndexBuffer(const std::vector<UINT>& indices, Graphics& gfx);
	int count;
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;

};