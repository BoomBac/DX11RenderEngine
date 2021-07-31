#pragma once
#include "Bindable.h"
class VertexShader;
class VertexLayout;

class InputLayout :
    public Bindable
{
public:
    InputLayout(ID3D11Device* pd, const VertexShader& vs, VertexLayout& vl);
    virtual void Bind(Graphics& gfx) override;
    void TBind(ID3D11DeviceContext& pc);
private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
};
