#include "Public/Render/Effect/Pass.h"
#include "Public/Render/ResManage/ShaderFactory.h"
#include "Public/Render/Drawable/Drawable.h"
#include "Public/Global.h"


void Pass::Render(const std::vector<Drawable*>& render_object)
{
	ApplyPass();
	for (auto& object : render_object)
	{
		object->Draw(*Global::getInstance()->G_Gfx);
	}
}

void Pass::SetRTVonly(ID3D11RenderTargetView* view)
{
	Global::getInstance()->G_Context->OMSetRenderTargets(1, &view, nullptr);
}

void Pass::SetDASonly(ID3D11DepthStencilView* depth)
{
	ID3D11RenderTargetView* null_view[] = { nullptr };
	Global::getInstance()->G_Context->OMSetRenderTargets(1, null_view, depth);
}

void Pass::SetRTVandDAS(ID3D11RenderTargetView* view, ID3D11DepthStencilView* depth)
{
	Global::getInstance()->G_Context->OMSetRenderTargets(1, &view, depth);
}

void Pass::AddVertexShader(const char* file_name)
{
	p_vertex_shader_ = ShaderFactory::getInstance().GetVShader(file_name);
}

void Pass::AddPixelShader(const char* file_name)
{
	p_pixel_shader_ = ShaderFactory::getInstance().GetPShader(file_name);
}

void Pass::SetShadeResource(ID3D11ShaderResourceView** view, UINT id)
{
	Global::getInstance()->G_Context->PSSetShaderResources(id, 1, view);
}

void Pass::AddDepthState(const D3D11_DEPTH_STENCIL_DESC& des)
{
	Global::getInstance()->G_Device->CreateDepthStencilState(&des, p_depth_state_.GetAddressOf());
}

void Pass::AddBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& des)
{
	//Global::getInstance()->G_Device->CreateBlendState(&des,p_blend_state.GetAddressOf());
}

void Pass::UpdateBuffer()
{
	Graphics* g = Global::getInstance()->G_Gfx;
	if (!b_shadow_pass)
	{
		g->UpdatePerFrameBuf(std::string("gView"), g->p_camera_->view_matrix());
		g->UpdatePerFrameBuf(std::string("gViewProj"), g->p_camera_->view_projection_matrix());
		g->UpdatePerFrameBuf(std::string("gCamreaPos"), g->p_camera_->location_f());
	}
	else
	{
		g->UpdatePerFrameBuf(std::string("gView"), g->p_light_camera->view_matrix());
		g->UpdatePerFrameBuf(std::string("gViewProj"), g->p_light_camera->view_projection_matrix());
		g->UpdatePerFrameBuf(std::string("gCamreaPos"), g->p_light_camera->location_f());
	}
}

void Pass::ApplyPass()
{
	if(p_vertex_shader_!=nullptr)
	p_vertex_shader_->Bind(*Global::getInstance()->G_Gfx);
	else
	{
		Global::getInstance()->G_Context->VSSetShader(nullptr, nullptr, 0u);
	}
	if(p_pixel_shader_ != nullptr)
	p_pixel_shader_->Bind(*Global::getInstance()->G_Gfx);
	else
	{
		Global::getInstance()->G_Context->PSSetShader(nullptr, nullptr, 0u);
	}
	if (p_depth_state_ != nullptr)
		Global::getInstance()->G_Context->OMSetDepthStencilState(p_depth_state_.Get(),1u);
	else
	{
		Global::getInstance()->G_Context->OMSetDepthStencilState(nullptr, 1u);
	}
	//if (p_blend_state != nullptr)
	//{
	//	FLOAT f[4]{ 1.f,1.f,1.f,1.f };
	//	Global::getInstance()->G_Context->OMSetBlendState(p_blend_state.Get(), f, 0xffffffff);
	//}

	//else
	//{
	//	Global::getInstance()->G_Context->OMSetDepthStencilState(nullptr, 1u);
	//}
	UpdateBuffer();
}
