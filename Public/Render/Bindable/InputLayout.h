#pragma once
#include "Bindable.h"
class VertexShader;
class VertexLayout;

class InputLayout :
    public Bindable
{
public:
    InputLayout(Graphics& gfx, const VertexShader& vs, VertexLayout& vl);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
};

