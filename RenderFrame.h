#pragma once
#include <qwindowdefs_win.h>
#include <wrl/client.h>

class ID3D11Device;
class ID3D11DeviceContext;
class IDXGISwapChain;
class ID3D11RenderTargetView;

class RenderFrame
{
public:
	RenderFrame(HWND hWnd);
	RenderFrame(const RenderFrame&) = delete;
	RenderFrame& operator=(const RenderFrame&) = delete;
	~RenderFrame();
	void EndFrame();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>pRenderTargetView = nullptr;
	HRESULT InitDx11(HWND hWnd);
	float* bg_color;

	void DrawTestGraph();
};

