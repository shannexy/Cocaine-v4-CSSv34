#include "Shared.hpp"

namespace Shared
{
	std::atomic< bool > m_bEject = false;
	std::atomic< bool > m_bMenu = false;
	std::atomic< bool > m_bSpeed = false;
	std::atomic< bool > m_bStuck = false;
	std::atomic< bool > m_bPanic = false;

	std::atomic< bool > m_bLoad = false;
	std::atomic< bool > m_bSave = false;

	Vars*				m_pVars = nullptr;
	std::string			m_strConfig;

	Timer::Timer( bool bStart )
	{
		if( bStart )
			Reset();
	}

	void Timer::Reset()
	{
		m_Start = clock::now();
	}

	long long Timer::Elapsed()
	{
		return std::chrono::duration_cast< mseconds >( clock::now() - m_Start ).count();
	}
}