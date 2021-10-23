#pragma once

#include <wrl/client.h>
#include <vector>
#include <map>
#include <qwindowdefs_win.h>

#include "vector3D.h"
#include "Public/Render/camera.h"
#include "Public/Tool/Subject.h"
#include <d3d11.h>
#include "Light/LightSet.h"



class ID3D11Device;
class ID3D11DeviceContext;
class IDXGISwapChain;
class ID3D11RenderTargetView;
class DepthStencil;
class Drawable;
class Light;
class SkyBox;
class SamplerState;

struct ShadowEffect
{
	float light_size;
	float max_bias;
	float light_near;
	float light_far;
};
struct MaterialProperty
{
	//Reflectance ratio
	DirectX::XMFLOAT3 albedo;
	float metallic;
	float roughness;
	DirectX::XMFLOAT3 padding;
};
enum class EAxisType
{
	kXAxis,
	kYAxis,
	kZAxis,
};

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
	Camera* p_camera_;
	Camera* p_light_camera;
	bool isRenderShaodw = true;
	ID3D11RenderTargetView* pp_render_targetview();
	//当前渲染视口选中的物体
	static Drawable* p_selected_object_;
	static Drawable* p_coordinate_;

	Drawable* p_light_ = nullptr;

	LightSet* p_scene_light_;
	LightShader* p_light_shader_;

	DirectX::XMMATRIX* p_light_view_projection_;
	LightMatrix* p_light_matrix_;
	ShadowEffect* p_shadow_effect_;
	MaterialProperty* p_material_property_;

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
	//主界面添加灯光接口
	void AddLight(ELightType light_type);
	Subject* outline_notify_;
	std::map<int, std::string> scene_outline_;
	std::string last_add_object_name_;
	void ResizeBackbuffer(const UINT& w, const UINT& h);
	void SetSelectObject(const int& index);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	ID3D11DepthStencilView* GetDepthStencilView();
	int GetWidth() const;
	int GetHeight() const;
	void SetRenderTargetView(ID3D11RenderTargetView* target_view);
	void SetDepthStencilView(ID3D11DepthStencilView* depth_view);
	//ID3D11ShaderResourceView** GetShadowMap();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>p_render_targetview_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>p_depth_stencil_view_ = nullptr;
	int width_;
	int height_;
	LightSet default_light_;
	LightShader default_light_shader_;
	std::unique_ptr<SamplerState> p_sampler_state_;
	std::unique_ptr<DepthStencil> p_depth_stencil_;
	HRESULT InitializeD3DBase(HWND hWnd, const UINT& w, const UINT& h);
	void InitializeSharedBindable();
	float* bg_color;
	// 容纳所有场景物体的容器,0 号元素是坐标轴
	std::vector<Drawable*> scene_objects_;
	// 将场景元素和他们的名字对应
	//int为其在scene_objects_中的索引，string则为其显示在ui上的名字
	ECameraMovementState cam_move_state_;
	void UpdateCameraMovement();
	std::vector<Camera*> camera_set_;
	void SetSelectObject(Drawable* object);

	ShadowEffect shadowParma;
	MaterialProperty materal_property_;
	std::unique_ptr<SkyBox>	p_sky_box_;
};

