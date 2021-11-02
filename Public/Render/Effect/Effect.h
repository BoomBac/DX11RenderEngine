#ifndef DX11ENGINE_RENDER_EFFECT_EFFECT_H
#define DX11ENGINE_RENDER_EFFECT_EFFECT_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <memory>
#include <Public/Render/Bindable/PixelShader.h>
#include <Public/Render/Effect/Pass.h>

enum class EEffectType
{
    kNone,
    kPBREffect,
    kShadow
};

class Effect
{
public:
    Effect(EEffectType type);
    void Render(const std::vector<Drawable*>& render_object);
    Pass* GetPass(UINT id);
private:
    DISALLOW_COPY_AND_ASSIGN(Effect)
    std::vector<std::unique_ptr<Pass>> pass_;
    EEffectType type_;
};


#endif //DX11ENGINE_RENDER_EFFECT_EFFECT_H
