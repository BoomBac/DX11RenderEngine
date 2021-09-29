#ifndef DX11ENGINE_RENDER_MODELRESFACTORY_H
#define DX11ENGINE_RENDER_MODELRESFACTORY_H


#include "Bindable/BindableInterface.h"
#include "ResManage/ResManage.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


class ModelResFactory : public ResManage
{
public:
    static ModelResFactory& GetInstance();
    std::vector<std::shared_ptr<BindableInterface>>& GetResource(const std::string& model_key);
    void AddResource(std::string model_key);
private:
   DISALLOW_COPY_AND_ASSIGN(ModelResFactory)
   ModelResFactory() {};
   std::map<std::string, std::vector<std::shared_ptr<BindableInterface>>> res_pool_;
   bool Exist(std::string model_key);
   bool Exist(std::string model_key, std::map<std::string, std::vector<std::shared_ptr<BindableInterface>>>::iterator& it);
};

#endif //DX11ENGINE_RENDER_MODELRESFACTORY_H

