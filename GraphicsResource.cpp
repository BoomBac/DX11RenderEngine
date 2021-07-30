#include "GraphicsResource.h"

ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& gfx) noexcept
{
	return gfx.pDeviceContext.Get();
}

ID3D11Device* GraphicsResource::GetDevice(Graphics& gfx) noexcept
{
	return gfx.pDevice.Get();
}
