#include "Public/Render/Effect/Effect.h"
#include "Public/Render/RenderToTexture.h"
#include <Public/Global.h>
#include <Public/Render/Material/Material.h>


namespace
{
	std::unique_ptr<RenderToTexture> rtt_depth;
	std::unique_ptr<RenderToTexture> rtt_back;
	std::unique_ptr<Material> mat;
}

Effect::Effect(EEffectType type)
{
	type_ = type;
	if (type == EEffectType::kPBREffect)
	{
		D3D11_TEXTURE2D_DESC tDesc;
		ZeroMemory(&tDesc, sizeof(tDesc));
		tDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		tDesc.Width = Global::getInstance()->G_Gfx->GetWidth();
		tDesc.Height = Global::getInstance()->G_Gfx->GetHeight();
		rtt_back = std::make_unique<RenderToTexture>();
		rtt_back->Initialize(Global::getInstance()->G_Gfx, ERTTUsage::kBackBuffer, tDesc);
		std::unique_ptr<Pass> ps = std::make_unique<Pass>();

		ps->AddPixelShader("pbr.cso");
		ps->AddVertexShader("VertexShader.cso");
		pass_.push_back(std::move(ps));
	}
	else if (type == EEffectType::kNone)
	{
		std::unique_ptr<Pass> ps = std::make_unique<Pass>();
		pass_.push_back(std::move(ps));
	}
	else if (type == EEffectType::kShadow)
	{
		rtt_depth = std::make_unique<RenderToTexture>();
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		rtt_depth->Initialize(Global::getInstance()->G_Gfx,ERTTUsage::kDepthBuffer,textureDesc);
		std::unique_ptr<Pass> ps_s = std::make_unique<Pass>();
		ps_s->b_shadow_pass = true;
		ps_s->AddVertexShader("VertexShader.cso");
		pass_.push_back(std::move(ps_s));
		std::unique_ptr<Pass> ps = std::make_unique<Pass>();
		ps->AddVertexShader("VertexShader.cso");
		ps->AddPixelShader("Lambert.cso");
		mat = std::make_unique<Material>();
		mat->LoadFromLib("Lambert.cso");
		D3D11_DEPTH_STENCIL_DESC das_des{};
		ZeroMemory(&das_des, sizeof(das_des));
		das_des.DepthEnable = true;
		das_des.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		ps->AddDepthState(das_des);
		pass_.push_back(std::move(ps));
	}
}

void Effect::Render(const std::vector<Drawable*>& render_object)
{
	auto g = Global::getInstance()->G_Gfx;

	if (type_ == EEffectType::kPBREffect)
	{
		pass_[0]->SetRTVandDAS(g->pp_render_targetview(), g->GetDepthStencilView());
		for (auto& p : pass_)
		{
			p->Render(render_object);
		}
		rtt_back->SetRenderTarget(g);
		rtt_back->ClearRenderTarget(g, 1.f, 1.f, 1.f, 1.f);
		for (auto& p : pass_)
		{
			p->Render(render_object);
		}
	}
	if (type_ == EEffectType::kNone)
	{
		for (auto& p : pass_)
		{
			p->Render(render_object);
		}
	}
	if (type_ == EEffectType::kShadow)
	{
		//render shadow-depth
		mat->SetMatrix4x4("light_view_projection", g->p_light_camera->view_projection_matrix());
		mat->SetMatrix4x4("light_view", g->p_light_camera->view_matrix());
		mat->SetMatrix4x4("light_projection", g->p_light_camera->projection_matrix());
		mat->SetFloat("u_LightSize", 0.4f);
		mat->SetFloat("u_LightBias", 0.001f);
		mat->SetFloat("u_LightNear", 1.f);
		mat->SetFloat("u_LightFar", 1000.f);
		mat->CommitAllBufferData();
		rtt_depth->SetRenderTarget(g);
		rtt_depth->ClearRenderTarget(g, 0.f, 0.f, 0.f, 1.f);
		pass_[0]->Render(render_object);
		//render scene
		pass_[1]->SetRTVandDAS(g->pp_render_targetview(),g->GetDepthStencilView());
		pass_[1]->SetShadeResource(rtt_depth->GetShaderResourceView(),5u);
		pass_[1]->SetShadeResource(rtt_back->GetShaderResourceView(),10u);
		pass_[1]->Render(render_object);
		ID3D11ShaderResourceView* r[] = { nullptr };
		Global::getInstance()->G_Context->PSSetShaderResources(5, 1, r);
		Global::getInstance()->G_Context->PSSetShaderResources(10, 1, r);
	}
}

Pass* Effect::GetPass(UINT id)
{
	return pass_[id].get();
}
