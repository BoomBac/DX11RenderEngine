#include "Public/Render/Shape/Model.h"
#include "Public/Render/ModelResFactory.h"
#include "Public/Render/Bindable/TransformBuffer.h"
#include "Public/Render/ResManage/TextureFactory.h"
#include <Public/Global.h>


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
	v_cons_buf_.world_matrix_ =DirectX::XMMatrixTranslation(world_location_.x,world_location_.y,world_location_.z);
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
	effects_.push_back(EEffectType::kPBREffect);
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
	v_cons_buf_.world_matrix_ =DirectX::XMMatrixTranslation(world_location_.x,world_location_.y,world_location_.z);
	//v_cons_buf_.camera_pos = gfx.p_camera_->location_f();
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);

	AddBind(std::move(vcb));

	p_mat_ = std::make_unique<Material>();
	p_mat_->LoadFromLib("pbr.cso");
	std::vector<std::string> t_l;
	t_l.push_back("rustediron2_basecolor.png");
	t_l.push_back("rustediron2_metallic.png");
	t_l.push_back("rustediron2_roughness.png");
	t_l.push_back("rustediron2_normal.png");
	p_mat_->InitTexture(t_l);
	effects_.push_back(EEffectType::kPBREffect);
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
	p_mat_->CommitAllTexture();
	Drawable::Draw(gfx);
}

const std::vector<std::unique_ptr<BindableInterface>>& Model::GetStaticBinds() const
{
	return binds_;
}

