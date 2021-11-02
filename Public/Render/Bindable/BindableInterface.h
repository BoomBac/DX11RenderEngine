#ifndef DX11ENGINE_RENDER_BINDABLE_BINDABLE_H
#define DX11ENGINE_RENDER_BINDABLE_BINDABLE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <d3d11.h>
#include <string>

#include "Public\Render\GraphicsResource.h"

enum class EBindableType
{
	kVertexBuffer,
	kIndexBuffer,
	kInputLayout,
	kVertexShader,
	kPixelShader,
	kConstantBuffer,
	kPixelConstantBuffer,
	kVetexConstantBuffer,
	kDepthStencilBuffer,
	kShaderResource,
	kRasterizerState,
	kDepthStencilState,
	kSamplerState
};

namespace
{
	int vc_i = 0;
	int pc_i = 0;
}
class BindableInterface : public GraphicsResource
{
public:
	~BindableInterface() = default;
	virtual void Bind(Graphics& gfx) = 0;
	virtual EBindableType GetType() const=0;
	int vc_buf_index_ = 0;
	int pc_buf_index = 0;
};

#endif //DX11ENGINE_RENDER_BINDABLE_BINDABLE_H