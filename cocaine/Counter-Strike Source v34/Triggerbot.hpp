#pragma once

#include "Valve.hpp"

namespace Feature
{
	class Triggerbot
	{
	public:
		Triggerbot();

		void				OnCreateMove( CUserCmd* pCmd );
		void				OnKeyEvent( UINT message, WPARAM wParam, LPARAM lParam );

	private:
		bool				PerformTrace( int random_seed );

	private:
		CUserCmd*			m_pCmd;
		C_CSPlayer*			m_pLocal;
		C_WeaponCSBaseGun*	m_pWeapon;
		CCSWeaponInfo*		m_pData;

		bool				m_bKeyPressed;
		bool				m_bFire;

		Shared::Timer		m_Timer;
	};
}