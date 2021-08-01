#pragma once
#include "Bindable.h"
#include <vector>


class IndexBuffer : public Bindable
{
public:
	IndexBuffer(const std::vector<UINT>& indices, Graphics& gfx);
	virtual void Bind(Graphics& gfx) override;
	int count;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
};