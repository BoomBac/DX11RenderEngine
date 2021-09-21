#pragma once
#include <qwindowdefs_win.h>
#include <wrl/client.h>
#include "vector3D.h"
#include <vector>
#include "Public/Render/camera.h"


class ID3D11Device;
class ID3D11DeviceContext;
class IDXGISwapChain;
class ID3D11RenderTargetView;
class DepthStencil;
class Drawable;

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
	void SetCameraTransformationW(const float& x, const float& y, const float& z);
	//设置摄像机位移
	void SetCameraTranslation(const float& x, const float& y, const float& z);
	void SetVPBackColor(float color[4]);
	void SetSelectedObjectTranslate(const CusMath::vector3d& t);
	void SetSelectedObjectRotation(const CusMath::vector3d& t);
	void SetSelectedObjectScale(const CusMath::vector3d& t);
	//从按键输入接收摄像机运动状态
	void UpdateCameraState(ECameraMovementState new_state);

	Camera camera;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>pRenderTargetView = nullptr;
	DepthStencil* dsbuffer;
	HRESULT InitDx11(HWND hWnd);
	float* bg_color;
	std::vector<Drawable*> SceneObjects;

	ECameraMovementState cam_move_state_;
	void UpdateCameraMovement();
};

