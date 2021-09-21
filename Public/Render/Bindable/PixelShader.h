#pragma once
#include "Bindable.h"


class PixelShader :
    public Bindable
{
public:
    PixelShader(Graphics& gfx,const std::string& path);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPShader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
};

