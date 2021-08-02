#pragma once
#include <qwindowdefs_win.h>
#include <wrl/client.h>

class ID3D11Device;
class ID3D11DeviceContext;
class IDXGISwapChain;
class ID3D11RenderTargetView;
class Box;

class Graphics
{
	friend class GraphicsResource;
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void DrawIndexed(const UINT& count);
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>pRenderTargetView = nullptr;
	HRESULT InitDx11(HWND hWnd);
	float* bg_color;
	Box* box = nullptr;
	Box* box1 = nullptr;
};

