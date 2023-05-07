#include "Triggerbot.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Config.hpp"

namespace Feature
{
	Triggerbot::Triggerbot()
		:	m_pCmd( nullptr ),
			m_pLocal( nullptr ),
			m_pWeapon( nullptr ),
			m_pData( nullptr ),
			m_bKeyPressed( false ),
			m_bFire( false )
	{

	}

	void Triggerbot::OnCreateMove( CUserCmd* pCmd )
	{
		if( !Config::Current->Triggerbot->Mode )
			return;

		if( Config::Current->Triggerbot->Mode == 2 )
		{
			if( !m_bKeyPressed )
			{
				m_bFire = false;
				m_Timer.Reset();
				return;
			}
		}

		m_pCmd = pCmd;

		if( !m_pCmd )
			return;

		m_pLocal = C_CSPlayer::GetLocalPlayer();

		if( !m_pLocal )
			return;

		if( m_pLocal->m_lifeState() != LIFE_ALIVE )
			return;

		m_pWeapon = m_pLocal->GetActiveWeapon();

		if( !m_pWeapon )
			return;

		if( m_pWeapon->IsMelee() )
			return;

		m_pData = m_pWeapon->GetCSWpnData();

		if( !m_pData )
			return;

		if( Config::Current->Triggerbot->Delay )
		{
			if( m_Timer.Elapsed() < Config::Current->Triggerbot->Delay )
				return;
		}

		if( Config::Current->Triggerbot->Accuracy == 2 ) // Seed
		{
			int seed = -1;

			for( int i = 0; i < 256; i++ )
			{
				if( PerformTrace( i ) )
				{
					seed = i;
					break;
				}
			}

			if( seed != -1 )
			{
				m_pCmd->random_seed = seed;

				while( ( MD5_PseudoRandom( m_pCmd->command_number ) & 255 ) != m_pCmd->random_seed )
					m_pCmd->command_number++;

				m_bFire = true;
				m_pCmd->buttons |= IN_ATTACK;
			}
		}
		else // Normal & Perfect
		{
			if( PerformTrace( m_pCmd->random_seed ) )
			{
				m_bFire = true;
				m_pCmd->buttons |= IN_ATTACK;
			}
		}

		if( m_bFire )
		{
			if( m_pLocal->m_iShotsFired() < Config::Current->Triggerbot->Burst )
				m_pCmd->buttons |= IN_ATTACK;
			else
				m_bFire = false;
		}
	}

	void Triggerbot::OnKeyEvent( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if( Config::Current->Triggerbot->Mode != 2 )
			return;

		if( Config::Current->Triggerbot->Key == 1 ) // Mouse 1
		{
			if( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK )
				m_bKeyPressed = true;
			else if( message == WM_LBUTTONUP )
				m_bKeyPressed = false;
		}
		else if( Config::Current->Triggerbot->Key == 2 ) // Mouse 2
		{
			if( message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK )
				m_bKeyPressed = true;
			else if( message == WM_RBUTTONUP )
				m_bKeyPressed = false;
		}
		else if( Config::Current->Triggerbot->Key == 4 ) // Mouse 3
		{
			if( message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK )
				m_bKeyPressed = true;
			else if( message == WM_MBUTTONUP )
				m_bKeyPressed = false;
		}
		else if( Config::Current->Triggerbot->Key == 5 ) // Mouse 4
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON1 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					m_bKeyPressed = true;
				else if( message == WM_XBUTTONUP )
					m_bKeyPressed = false;
			}
		}
		else if( Config::Current->Triggerbot->Key == 6 ) // Mouse 5
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON2 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					m_bKeyPressed = true;
				else if( message == WM_XBUTTONUP )
					m_bKeyPressed = false;
			}
		}
		else
		{
			if( wParam == Config::Current->Triggerbot->Key )
			{
				if( message == WM_KEYDOWN || message == WM_SYSKEYDOWN )
					m_bKeyPressed = true;
				else if( message == WM_KEYUP || message == WM_SYSKEYUP )
					m_bKeyPressed = false;
			}
		}
	}

	bool Triggerbot::PerformTrace( int random_seed )
	{
		auto viewangles( m_pCmd->viewangles );
		auto start( m_pLocal->EyePosition() );

		Vector3 end;

		if( Config::Current->Triggerbot->Accuracy )
			Source::m_pAccuracy->ApplySpreadFix( m_pWeapon, random_seed, viewangles, viewangles, 2, true );

		Source::m_pAccuracy->ApplyRecoilFix( m_pLocal, viewangles, true );

		AngleVectors( viewangles, &end );

		end = end * m_pData->m_flRange + start;

		auto direction = end - start;
		VectorNormalize( direction );

		C_BaseEntity* hit( nullptr );

		int hitbox = -1;
		int hitgroup = -1;

		if( Config::Current->Triggerbot->AutoWall )
		{
			if( !Source::m_pAccuracy->CanPenetrate( start, end, Config::Current->Triggerbot->MinDamage, Config::Current->Triggerbot->Target, nullptr, &hitbox, &hitgroup, &hit ) )
				return false;
		}
		else
		{
			Ray_t ray;
			trace_t tr;

			CTraceFilterSimple trace( m_pLocal );

			ray.Set( start, end );

			Source::m_pEngineTrace->TraceRay( ray, 0x46004003, &trace, &tr );
			Valve::ClipTraceToPlayers( start, end + direction * 40.0f, 0x46004003, &trace, &tr );

			hitbox = tr.hitbox;
			hitgroup = tr.hitgroup;
			hit = tr.m_pEnt;
		}

		auto player = ToCSPlayer( hit );

		if( !player )
			return false;

		if( Config::Current->Triggerbot->Target == 1 )
		{
			if( player->m_iTeamNum() == m_pLocal->m_iTeamNum() )
				return false;
		}
		else if( Config::Current->Triggerbot->Target == 2 )
		{
			if( player->m_iTeamNum() != m_pLocal->m_iTeamNum() )
				return false;
		}

		if( ( Config::Current->Triggerbot->Head && hitgroup == 1 ) ||
			( Config::Current->Triggerbot->Chest && hitgroup == 2 ) ||
			( Config::Current->Triggerbot->Stomach && hitgroup == 3 ) ||
			( Config::Current->Triggerbot->Arms && ( hitgroup == 4 || hitgroup == 5 ) ) ||
			( Config::Current->Triggerbot->Legs && ( hitgroup == 6 || hitgroup == 7 ) ) )
			return true;

		return false;
	}
}