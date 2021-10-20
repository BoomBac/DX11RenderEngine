#include "Public/Render/Shape/Model.h"
#include "Public/Render/ModelResFactory.h"
#include "Public/Render/Bindable/TransformBuffer.h"
#include "Public/Render/ResManage/TextureFactory.h"


using BindItem = std::unique_ptr<BindableInterface>;


namespace 
{
	//Texture tex;
}

Model::Model(Graphics& gfx)
{
	//visiblity_ = false;
	for (auto sp_bind : ModelResFactory::GetInstance().GetResource("zzz.obj"))
	{
		std::unique_ptr<BindableInterface> up_bind(sp_bind.get());
		binds_.push_back(std::move(up_bind));
	}
	for (auto& i : binds_)
	{
		if (i->GetType() == EBindableType::kIndexBuffer)
		{
			indexbuffer = dynamic_cast<IndexBuffer*>(i.get());
			break;
		}
	}
	view = gfx.p_camera_->view_matrix();
	projection = gfx.p_camera_->projection_matrix();
	world_location_ = {0.f,0.f,0.f};
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	v_cons_buf_.mvp_matrix_ =
	{
		DirectX::XMMatrixTranslation(world_location_.x,world_location_.y,world_location_.z) *
		view *
		projection
	};
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
}

Model::Model(Graphics& gfx, const char* res_key)
{
	res_key_ = res_key;
	for (auto sp_bind : ModelResFactory::GetInstance().GetResource(res_key_))
	{
		std::unique_ptr<BindableInterface> up_bind(sp_bind.get());
		binds_.push_back(std::move(up_bind));
	}
	for (auto& i : binds_)
	{
		if (i->GetType() == EBindableType::kIndexBuffer)
		{
			indexbuffer = dynamic_cast<IndexBuffer*>(i.get());
			break;
		}
	}
	view = gfx.p_camera_->view_matrix();
	projection = gfx.p_camera_->projection_matrix();
	world_location_ = { 0.f,0.f,0.f };
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	v_cons_buf_.mvp_matrix_ =
	{
		DirectX::XMMatrixTranslation(world_location_.x,world_location_.y,world_location_.z) *
		view *
		projection
	};
	//v_cons_buf_.camera_pos = gfx.p_camera_->location_f();
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
	//"Y:\Project_VS2019\DX11RenderEngine\Res\rustediron1-alt2-Unreal-Engine\rustediron2_basecolor.png"
	std::unique_ptr<ShaderResource> difrsv = std::make_unique<ShaderResource>(TextureFactory::GetInstance().GetTexture("rustediron2_basecolor.png")
		->GetTextureResourceView(),ETextureType::kDiffuse);
	AddBind(std::move(std::make_unique<ShaderResource>(difrsv->GetResourceView(), ETextureType::kDiffuse)));
	texture_set_.insert(std::make_pair<ETextureType, std::unique_ptr<ShaderResource>>(ETextureType::kDiffuse, std::move(difrsv)));

	std::unique_ptr<ShaderResource> metrsv = std::make_unique<ShaderResource>(TextureFactory::GetInstance().GetTexture("rustediron2_metallic.png")
		->GetTextureResourceView(), ETextureType::kMetallic);
	AddBind(std::move(std::make_unique<ShaderResource>(metrsv->GetResourceView(), ETextureType::kMetallic)));
	texture_set_.insert(std::make_pair<ETextureType, std::unique_ptr<ShaderResource>>(ETextureType::kMetallic, std::move(metrsv)));

	std::unique_ptr<ShaderResource> norrsv = std::make_unique<ShaderResource>(TextureFactory::GetInstance().GetTexture("rustediron2_normal.png")
		->GetTextureResourceView(), ETextureType::kNormal);
	AddBind(std::move(std::make_unique<ShaderResource>(norrsv->GetResourceView(), ETextureType::kNormal)));
	texture_set_.insert(std::make_pair<ETextureType, std::unique_ptr<ShaderResource>>(ETextureType::kNormal, std::move(norrsv)));

	std::unique_ptr<ShaderResource> roursv = std::make_unique<ShaderResource>(TextureFactory::GetInstance().GetTexture("rustediron2_roughness.png")
		->GetTextureResourceView(), ETextureType::kRoughness);
	AddBind(std::move(std::make_unique<ShaderResource>(roursv->GetResourceView(), ETextureType::kRoughness)));
	texture_set_.insert(std::make_pair<ETextureType, std::unique_ptr<ShaderResource>>(ETextureType::kRoughness, std::move(roursv)));
}

Model::~Model()
{
	for (auto& up : binds_)
	{
		up.release();
	}
}

void Model::Draw(Graphics& gfx)
{
	Drawable::Draw(gfx);
}

const std::vector<std::unique_ptr<BindableInterface>>& Model::GetStaticBinds() const
{
	return binds_;
}

