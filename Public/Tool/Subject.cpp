#include "Public\Tool\Subject.h"


	
//无需知道观察者具体身份，一并发送通知


void Subject::AddObserver(ObserverInterface* observer)
{
	observer_list_.push_back(observer);
}

void Subject::RemoveObserver(ObserverInterface* observer)
{
	auto it = observer_list_.begin();
	for (; it != observer_list_.end(); it++)
	{
		if (*it == observer) break;
	}
	observer_list_.erase(it);
}

void Subject::NotifyObserver(bool is_add)
{
	if (observer_list_.empty()) return;
	for (auto o : observer_list_)
	{
		o->OnOutlineChanged(is_add);
	}
}
