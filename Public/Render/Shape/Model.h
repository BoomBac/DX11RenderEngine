#ifndef DX11ENGINE_RENDER_SHAPE_MODEL_H
#define DX11ENGINE_RENDER_SHAPE_MODEL_H


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "Public\Render\Drawable\Drawable.h"
#include "Public\Render\Shape\Shape.hpp"
#include <Public/Render/Texture.h>
#include <Public/Render/Bindable/ShaderResource.h>
#include <Public/Render/Material/Material.h>


class Model :
	public Shape<Model>
{
public:
	Model() {};
	Model(Graphics& gfx,const char* res_key);
	Model (Graphics& gfx);
	~Model();
	virtual void Draw(Graphics& gfx) override;
	//TODO(): add copy()
	std::unique_ptr<Material> p_mat_;

private:
	DISALLOW_COPY_AND_ASSIGN(Model)

	std::vector<std::unique_ptr<BindableInterface>> binds_;
	virtual const std::vector<std::unique_ptr<BindableInterface>>& GetStaticBinds() const override;
	const char* res_key_ = "zzz.obj";
};

#endif //DX11ENGINE_RENDER_SHAPE_MODEL_H