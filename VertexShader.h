#pragma once
#include "Bindable.h"
#include <string>

class VertexShader :
    public Bindable
{
    friend class InputLayout;
public:
    VertexShader(ID3D11Device* pd,const std::string& path);
    virtual void Bind(Graphics& gfx) override;
    void TBind(ID3D11DeviceContext& pc);
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVShader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
};

