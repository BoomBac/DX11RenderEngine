#ifndef DX11ENGINE_TOOL_OBSERVERINTERFACE_H
#define DX11ENGINE_TOOL_OBSERVERINTERFACE_H


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

class ObserverInterface
{
public:
    ObserverInterface() {};
    virtual void OnOutlineChanged(bool is_add) = 0;
private:

    DISALLOW_COPY_AND_ASSIGN(ObserverInterface)
};



#endif //DX11ENGINE_TOOL_OBSERVERINTERFACE_H