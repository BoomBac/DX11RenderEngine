#pragma once
#include <string>
constexpr float PI = 3.1415926f;

#define Debug(msg) OutputDebugString(ToWide(std::string("MyDebug : ") + msg).c_str());

#define DEBUG_HEADER std::string()

inline std::wstring ToWide(const std::string& narrow)
{
	wchar_t wide[512];
	mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
	return wide;
}

inline std::string ToNarrow(const std::wstring& wide)
{
	char narrow[512];
	wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);
	return narrow;
}
inline float DegToRad(const float& angle)
{
	return angle * PI / 180.f;
}
inline float RadToDeg(const float& radius)
{
	return radius * 180.f / PI;
}
