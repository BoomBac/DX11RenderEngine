#pragma once
#include <memory>
#include <vector>
#include "IndexBuffer.h"


class Graphics;
class Bindable;


class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	void Draw(Graphics& gfx);
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
	~Drawable() = default;
protected:
	std::vector<std::unique_ptr<Bindable>> binds;
	std::unique_ptr<IndexBuffer> indexbuffer;
};

