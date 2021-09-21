#include "Public/Tool/Timer.h"

using namespace RE;
using namespace std::chrono;

RE::Timer::Timer()
{
	lastMark = steady_clock::now();
}

float RE::Timer::Mark()
{
	const auto old = lastMark;
	lastMark = steady_clock::now();
	const duration<float> inteval = lastMark - old;
	return inteval.count();
}

float RE::Timer::Peek() const
{
	return duration<float>(steady_clock::now() - lastMark).count();
}
