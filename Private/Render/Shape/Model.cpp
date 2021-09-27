#include "Public/Render/Shape/Model.h"
#include "Public/Render/ModelResFactory.h"
#include "Public/Render/Bindable/TransformBuffer.h"

using BindItem = std::unique_ptr<BindableInterface>;

Model::Model(Graphics& gfx)
{
	//visiblity_ = false;
	for (auto sp_bind : ModelResFactory::GetResource(0))
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
	view = gfx.camera_.view_matrix();
	projection = gfx.camera_.projection_matrix();
	world_location_ = {0.f,0.f,0.f};
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	transform =
	{
		DirectX::XMMatrixTranslation(world_location_.x,world_location_.y,world_location_.z) *
		view *
		projection
	};
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
}

Model::~Model()
{
	for (auto& up : binds_)
	{
		up.release();
	}
}

const std::vector<std::unique_ptr<BindableInterface>>& Model::GetStaticBinds() const
{
	return binds_;
}

