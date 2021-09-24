#pragma once

#include <wrl/client.h>
#include <vector>
#include <qwindowdefs_win.h>

#include "vector3D.h"
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

	void SetVPBackColor(float color[4]);
	void SetSelectedObjectTranslate(const CusMath::vector3d& t);
	void SetSelectedObjectRotation(const CusMath::vector3d& t);
	void SetSelectedObjectScale(const CusMath::vector3d& t);
	//从按键输入接收摄像机运动状态
	void UpdateCameraState(ECameraMovementState new_state);
	Camera camera_;
	ID3D11RenderTargetView* pp_render_targetview();
	//当前渲染视口选中的物体
	static Drawable* p_selected_object_;
	static Drawable* p_coordinate_;

	void SetCoordinateType(bool is_world);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>p_render_targetview_ = nullptr;
	DepthStencil* dsbuffer;
	HRESULT InitDx11(HWND hWnd);
	float* bg_color;
	std::vector<Drawable*> SceneObjects;

	ECameraMovementState cam_move_state_;
	void UpdateCameraMovement();
};

