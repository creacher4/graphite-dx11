#pragma once
#include <chrono>

class Timer
{
public:
	void Reset();
	float GetDeltaTime();

private:
	std::chrono::steady_clock::time_point m_prev = std::chrono::steady_clock::now();
};