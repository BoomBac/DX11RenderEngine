#pragma once
#include "GraphicsResource.h"
#include <d3d11.h>

class Bindable : public GraphicsResource
{
public:
	~Bindable() = default;
	virtual void Bind(Graphics& gfx) = 0;
protected:

};
