#pragma once
#include <chrono>

namespace RE
{
	class Timer
	{
	public:
		Timer();
		//mark��ǰʱ�䣬�����ص�ǰ��Ǻ���һ�εļ��
		float Mark();
		//���ص�ǰʱ�����һ�α�ǵļ��
		float Peek() const;
	private:
		//��һ�α�ǵ�ʱ���
		std::chrono::steady_clock::time_point lastMark;
	};
}


