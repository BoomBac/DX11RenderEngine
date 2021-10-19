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
	textures_.push_back(TextureFactory::GetInstance().GetTexture("Depth.png"));
	//gfx.GetContext()->PSSetShaderResources(0, 1, textures_[0]->GetTextureResourceViewAddress());
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
	textures_.push_back(TextureFactory::GetInstance().GetTexture("Depth.png"));
	//gfx.GetContext()->PSSetShaderResources(0, 1, textures_[0]->GetTextureResourceViewAddress());
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


	//gfx.GetContext()->PSSetShaderResources(0, 1, textures_[0]->GetTextureResourceViewAddress());
	Drawable::Draw(gfx);
}

const std::vector<std::unique_ptr<BindableInterface>>& Model::GetStaticBinds() const
{
	return binds_;
}

