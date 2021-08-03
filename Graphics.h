#pragma once
#include <qwindowdefs_win.h>
#include <wrl/client.h>
#include "vector3D.h"

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
	//每帧运行
	void EndFrame();
	//供drawable调用
	void DrawIndexed(const UINT& count);

	//与UI交互的接口
	//提供给renderViewPort修改Drawable的view矩阵
	void SetCameraTransformation(const float& x, const float& y, const float& z);
	void SetVPBackColor(float color[4]);
	void SetSelectedObjectTranslate(const CusMath::vector3d& t);
	void SetSelectedObjectRotation(const CusMath::vector3d& t);
	void SetSelectedObjectScale(const CusMath::vector3d& t);

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

