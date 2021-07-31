#pragma once
#include "Bindable.h"
#include <string>

class PixelShader :
    public Bindable
{
public:
    PixelShader(ID3D11Device* pd,const std::string& path);
    virtual void Bind(Graphics& gfx) override;
    void TBind(ID3D11DeviceContext& pc);
private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPShader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
};

