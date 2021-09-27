#pragma once

#include <wrl/client.h>
#include <vector>
#include <map>
#include <qwindowdefs_win.h>

#include "vector3D.h"
#include "Public/Render/camera.h"
#include "Public/Tool/Subject.h"


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

	//从按键输入接收摄像机运动状态
	void UpdateCameraState(ECameraMovementState new_state);
	Camera camera_;
	ID3D11RenderTargetView* pp_render_targetview();
	//当前渲染视口选中的物体
	static Drawable* p_selected_object_;
	static Drawable* p_coordinate_;

	void SetCoordinateType(bool is_world);
	//if world coord,return true
	bool GetCoordinateType() const;
	//这部分与Qt框架交互
	void SetSelectedObjectTranslate(const CusMath::vector3d& t);
	void SetSelectedObjectRotation(const CusMath::vector3d& t);
	void SetSelectedObjectScale(const CusMath::vector3d& t);
	//工厂类添加元素
	void AddSceneObject(Drawable* object, std::string object_name);
	
	void DeleteSceneObject(int index);
	//在构造时添加模型，最多16个，数量到RenderEngine修改
	void InitSceneObject();
	//将构造时创建的物品添加到outline。
	int InitOutline(std::string* item_name);

	Subject* outline_notify_;
	std::map<int, std::string> scene_outline_;
	std::string last_add_object_name_;

	void SetSelectObject(const int& index);
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>p_render_targetview_ = nullptr;
	DepthStencil* dsbuffer;
	HRESULT InitDx11(HWND hWnd);
	float* bg_color;
	// 容纳所有场景物体的容器,0 号元素是坐标轴
	std::vector<Drawable*> scene_objects_;
	// 将场景元素和他们的名字对应
	//int 为其在scene_objects_中的索引，string则为其显示在ui上的名字

	ECameraMovementState cam_move_state_;
	void UpdateCameraMovement();

	void SetSelectObject(Drawable* object);
};

