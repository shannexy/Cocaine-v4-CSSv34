#pragma once

#include "Valve.hpp"

namespace Feature
{
	class Aimbot
	{
	public:
		Aimbot();

		void				OnCreateMove( CUserCmd* pCmd );
		void				OnKeyEvent( UINT message, WPARAM wParam, LPARAM lParam );

	private:
		void				ChangeTarget();

		bool				IsTargetGood( C_CSPlayer* pTarget );

		void				ApplyStepSmooth( Vector3& vAim );
		void				ApplyLinearSmooth( Vector3& vAim );

		Vector3				MakeVector( const Vector3& angles );
		float				GetFOV( const Vector3& va, const Vector3& src, const Vector3& dest );

	private:
		bool				m_bKeyPressed;
		bool				m_bChangeTarget;

		CUserCmd*			m_pCmd;
		C_CSPlayer*			m_pLocal;
		C_WeaponCSBaseGun*	m_pWeapon;
		CCSWeaponInfo*		m_pData;

		C_CSPlayer*			m_pTarget;
		Vector3				m_vTarget;
		
		Shared::Timer		m_Timer;
	};
}