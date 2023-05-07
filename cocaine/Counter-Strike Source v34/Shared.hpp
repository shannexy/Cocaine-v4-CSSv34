#pragma once

#include <windows.h>

#include <atomic>
#include <cstdint>
#include <chrono>
#include <string>

namespace Shared
{
	struct Vars
	{
		char m_loader[MAX_PATH] = { 0 };
	};

	extern std::atomic< bool > m_bEject;
	extern std::atomic< bool > m_bMenu;
	extern std::atomic< bool > m_bSpeed;
	extern std::atomic< bool > m_bStuck;
	extern std::atomic< bool > m_bPanic;

	extern std::atomic< bool > m_bLoad;
	extern std::atomic< bool > m_bSave;

	extern Vars* m_pVars;
	extern std::string			m_strConfig;

	class Timer
	{
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::milliseconds mseconds;

	public:
		Timer(bool bStart = false);

		void Reset();
		long long Elapsed();

	private:
		clock::time_point m_Start;
	};
}