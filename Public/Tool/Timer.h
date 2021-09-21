#pragma once
#include <chrono>

namespace RE
{
	class Timer
	{
	public:
		Timer();
		//mark当前时间，并返回当前标记和上一次的间隔
		float Mark();
		//返回当前时间和上一次标记的间隔
		float Peek() const;

	private:
		//上一次标记的时间点
		std::chrono::steady_clock::time_point lastMark;
	};
}
