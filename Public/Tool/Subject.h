#ifndef DX11ENGINE_TOOL_SUBJECT_H
#define DX11ENGINE_TOOL_SUBJECT_H


#include <vector>
#include "ObserverInterface.h"


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

//这是一个通知者类，和Observer类一起负责 view和 graphics间的通信
//主要就是graphics动态地增删场景物体，viewport 知道这种变化，并且通知到ui
// graphics维护一个public Subject*
class Subject
{
	friend class Graphics;
public:
	void AddObserver(ObserverInterface* observer);
	void RemoveObserver(ObserverInterface* observer);
	//通知，遍历监听者/观察者列表，依次执行他们的通知处理函数
private:
	void NotifyObserver(bool is_add);
	std::vector<ObserverInterface*> observer_list_;
};


#endif //DX11ENGINE_TOOL_SUBJECT_H