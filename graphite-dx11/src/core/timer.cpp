#include "timer.h"

void Timer::Reset()
{
	m_prev = std::chrono::steady_clock::now();
}

float Timer::GetDeltaTime()
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> delta = now - m_prev;
	m_prev = now;
	return delta.count();
}