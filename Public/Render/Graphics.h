#pragma once

#include <d3d11.h>

#include <wrl/client.h>
#include <vector>
#include <map>
#include <qwindowdefs_win.h>

#include "vector3D.h"

#include "Public/Render/camera.h"
#include "Public/Tool/Subject.h"
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
class Coordinate;
class Light;
template <class T> 
class VConstantBuffer;
enum class EEffectType;
template <class T>
class PConstantBuffer;



struct ShadowEffect
{
	float light_size;
	float max_bias;
	float light_near;
	float light_far;
};

struct VCommonStruct
{
	DirectX::XMMATRIX gView;
	DirectX::XMMATRIX gProj;
	DirectX::XMMATRIX gViewProj;
	DirectX::XMFLOAT3 gCamreaPos;
	float padding;
};
struct PCommonStruct
{
	LightSet gLightInfo;
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
	//update the VertexConstbuffer(slot 0) in ShaderHeader 
	void CommitPerFrameBuf();
	void UpdatePerFrameBuf(std::string& val_name,const DirectX::XMMATRIX& mat);
	void UpdatePerFrameBuf(std::string& val_name,const DirectX::XMFLOAT3& f3);


	//从按键输入接收摄像机运动状态
	void UpdateCameraState(ECameraMovementState new_state);
	Camera* p_camera_;
	Camera* p_light_camera;
	bool isRenderShaodw = true;
	ID3D11RenderTargetView* pp_render_targetview();
	//当前渲染视口选中的物体
	static Drawable* p_selected_object_;
	std::unique_ptr<Coordinate> p_coordinate_;
	Light* p_light_;

	LightSet* p_scene_light_;
	LightShader* p_light_shader_;

	DirectX::XMMATRIX* p_light_view_projection_;
	LightMatrix* p_light_matrix_;
	ShadowEffect* p_shadow_effect_;


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
	std::unique_ptr<VConstantBuffer<VCommonStruct>> p_com_vcons_buf_;
	std::unique_ptr<PConstantBuffer<PCommonStruct>> p_com_pcons_buf_;
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
	std::unique_ptr<SkyBox>	p_sky_box_;
	std::map<EEffectType, std::vector<Drawable*>> effect_bucket_;
	void AdjustRenderQueue();
	VCommonStruct common_v_cbuf_src;
	PCommonStruct common_p_cbuf_src;
	void UpdatePerFrameBuf();
};

