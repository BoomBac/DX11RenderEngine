#pragma once
#include "Bindable.h"


class VertexShader :
    public Bindable
{
    friend class InputLayout;
public:
    VertexShader(Graphics& gfx,const std::string& path);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVShader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
};

