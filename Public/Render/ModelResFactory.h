#ifndef DX11ENGINE_RENDER_MODELRESFACTORY_H
#define DX11ENGINE_RENDER_MODELRESFACTORY_H


#include "Bindable/BindableInterface.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

class ModelResFactory
{
public:
    ModelResFactory(Graphics* gfx);
    static std::vector<std::shared_ptr<BindableInterface>>& GetResource(const int& model_key);
    static bool AddResource(int model_key);
    static void LoadFile(std::string file_path);
private:
    static std::map<int, std::vector<std::shared_ptr<BindableInterface>>> res_pool_;
    static bool Exist(int model_key);
    static bool Exist(int model_key, std::map<int, std::vector<std::shared_ptr<BindableInterface>>>::iterator& it);
    static Graphics* gfx_;
};

#endif //DX11ENGINE_RENDER_MODELRESFACTORY_H

