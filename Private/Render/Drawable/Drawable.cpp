#include "Public/Render/Drawable/Drawable.h"
#include "Public/Tool/Utili.h"
#include "Public/Render/Graphics.h"
#include "Public/Render/Bindable/Bindable.h"
#include "Public/Render/Bindable/IndexBuffer.h"


namespace dx = DirectX;
//eyePos,lookAt,upDirection   test
DirectX::XMMATRIX Drawable::view;
DirectX::XMMATRIX Drawable::projection;

DirectX::XMMATRIX Drawable::cameraTranslate = DirectX::XMMatrixTranslation(0.f, 0.f, 50.f);
DirectX::XMMATRIX Drawable::cameraRotation = DirectX::XMMatrixIdentity();
DirectX::XMVECTOR Drawable::g_camera_forward = { 0.f,0.f,-1.f };
DirectX::XMVECTOR Drawable::g_camera_right = {-1.f,0.f,0.f};
DirectX::XMVECTOR Drawable::g_camera_up = {0.f,1.f,0.f};

static dx::XMVECTOR g_camera_location = { 0.f,0.f,50.f };

void Drawable::Draw(Graphics& gfx)
{
	view = gfx.camera.view_matrix();
	for (auto& i : binds)
	{
		i->Bind(gfx);
	}
	for (auto& i : GetStaticBinds())
	{
		i->Bind(gfx);
	}
	Update();
	gfx.DrawIndexed(indexbuffer->count);
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind)
{
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx)
{
	indexbuffer = ibf.get();
	indexbuffer->Bind(gfx);
}

WorldTransform& Drawable::GetTransform()
{
	return transform;
}

void Drawable::UpdateCameraTransformation(const DirectX::XMMATRIX& tranf)
{
	//cameraRotation = tranf * cameraRotation;
	g_camera_forward = dx::XMVector3Transform(g_camera_forward, tranf);//tranf * g_camera_direction ;
	g_camera_right = dx::XMVector3Transform(g_camera_right, tranf);//tranf * g_camera_direction ;
	g_camera_up = dx::XMVector3Transform(g_camera_up, tranf);//tranf * g_camera_direction ;
	//view = view * tranf;

	char buf[4];
	std::string str;
	for (int i = 0; i < 3; i++)
	{
		sprintf(buf, "%f", g_camera_forward.m128_f32[i]);
		str.append(buf);
		str.append(", ");
	}
	Debug("CameraForward:" + str + "\n");
}
void Drawable::UpdateCameraTransformationW(const DirectX::XMMATRIX& tranf, float detla)
{	
	g_camera_forward = dx::XMVector3Transform(g_camera_forward, tranf);//tranf * g_camera_direction ;
	g_camera_right = dx::XMVector3Transform(g_camera_right, tranf);//tranf * g_camera_direction ;
	g_camera_up = dx::XMVector3Transform(g_camera_up, tranf);
}


// 前进方向为z，那么在世界空间的前进距离就是物体坐标系的前进方向乘其前方向的世界空间方向
void Drawable::UpdateCameraTranslation(const DirectX::XMMATRIX& tranf)
{	
//	view = view * tranf;
	cameraTranslate = tranf * cameraTranslate ;
	//前进
	//
	if (g_camera_forward.m128_f32[2] < 0.f)
	g_camera_location = dx::XMVector3Transform(g_camera_location, DirectX::XMMatrixTranslation( - g_camera_forward.m128_f32[0],
		- g_camera_forward.m128_f32[1],+ g_camera_forward.m128_f32[2]));
	else
	{
		g_camera_location = dx::XMVector3Transform(g_camera_location, DirectX::XMMatrixTranslation(-g_camera_forward.m128_f32[0],
			g_camera_forward.m128_f32[1], +g_camera_forward.m128_f32[2]));
	}

	char buf[4];
	std::string str;
	for (int i = 0; i < 3; i++)
	{
		sprintf(buf, "%f", g_camera_location.m128_f32[i]);
		str.append(buf);
		str.append(" ");
	}
	Debug("camera_location: " + str + '\n');
	//std::string str;
	//for (int i = 0; i < 3; i++)
	//{
	//	sprintf(buf, "%f", g_camera_forward.m128_f32[i]);
	//	str.append(buf);
	//	str.append(", ");
	//}
	//Debug("CameraDirection:" + str + "\n");
}

void Drawable::SetActorLocation(const CusMath::vector3d& t)
{
	Location = t;
}

void Drawable::SetActorRotation(const CusMath::vector3d& r)
{
	Rotation = r;
}

void Drawable::SetActorScale(const CusMath::vector3d& s)
{
	Scale = s;
}

