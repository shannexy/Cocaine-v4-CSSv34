#include "Aimbot.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Config.hpp"

namespace Feature
{
	Aimbot::Aimbot()
		:	m_bKeyPressed( false ),
			m_bChangeTarget( false ),
			m_pCmd( nullptr ),
			m_pLocal( nullptr ),
			m_pWeapon( nullptr ),
			m_pData( nullptr ),
			m_pTarget( nullptr )
	{

	}

	/*Vector3 Predict( const Vector3& eye, const Vector3& target, const Vector3& target_speed )
	{
		Vector3 me = eye;

		me -= target;

		float a = target_speed.LengthSqr();
		float b = ( -2.0f * me.x * target_speed.x ) + ( -2.0f * me.y * target_speed.y ) + ( -2.0f * me.z * target_speed.z );
		float c = me.LengthSqr();

		me += target;

		double subsquare = b * b - 4.0f * a * c;

		if( subsquare < 0.0 )
			return vec3_zero;

		float aa = ( float )( ( -b + std::sqrt( subsquare ) ) / ( 2.0f * a ) );
		float bb = ( float )( ( -b - std::sqrt( subsquare ) ) / ( 2.0f * a ) );

		Vector3 result;

		if( aa >= 0.0f && !( bb >= 0.0f && bb < aa ) )
			result = target + target_speed * aa;
		else if( bb >= 0.0f )
			result = target + target_speed * bb;

		return result;
	}*/

	void Aimbot::OnCreateMove( CUserCmd* cmd )
	{
		if( !Config::Current->Aimbot->Mode ) // Off
			return;

		if( Config::Current->Aimbot->Mode == 2 ) // On Press
		{
			if( !m_bKeyPressed )
			{
				m_bChangeTarget = true;
				m_Timer.Reset();
				return;
			}
		}

		m_pCmd = cmd;

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

		if( !m_pWeapon->m_iClip1() )
		{
			if( Config::Current->Aimbot->AutoReload )
			{
				m_pCmd->buttons &= ~IN_ATTACK;
				m_pCmd->buttons |= IN_RELOAD;
			}

			return;
		}

		m_pData = m_pWeapon->GetCSWpnData();

		if( !m_pData )
			return;

		if( m_bChangeTarget )
			ChangeTarget();

		if( IsTargetGood( m_pTarget ) )
		{
			if( Config::Current->Aimbot->Delay )
			{
				if( m_Timer.Elapsed() < Config::Current->Aimbot->Delay )
					return;
			}

			if( Config::Current->Aimbot->Duration )
			{
				if( m_Timer.Elapsed() > Config::Current->Aimbot->Duration )
					return;
			}

			if( Config::Current->Aimbot->SpotRandomize )
			{
				Valve::RandomSeed( ( cmd->random_seed & 255 ) + 1 );

				float flRandom = Valve::RandomFloat( -1.5f, 1.5f );

				m_vTarget.x += flRandom;
				m_vTarget.y += flRandom;
				m_vTarget.z += flRandom;
			}

			/*float flDistance = m_vTarget.DistTo( m_pLocal->EyePosition() );

			m_vTarget += m_pTarget->m_vecVelocity() / flDistance;
			// m_vTarget -= m_pLocal->m_vecVelocity() / flDistance;

			m_vTarget += m_pTarget->m_vecVelocity() * Source::m_pGlobalVars->interval_per_tick;*/

			// m_vTarget = Predict( m_pLocal->EyePosition(), m_vTarget, m_pTarget->m_vecVelocity() );

			auto distance = m_vTarget.DistTo( m_pLocal->EyePosition() );

			m_vTarget += m_pTarget->m_vecVelocity() / distance;
			m_vTarget -= m_pLocal->m_vecVelocity() / distance;

			auto vDirection = m_vTarget - m_pLocal->EyePosition();

			Vector3 vAim;

			VectorNormalize( vDirection );
			VectorAngles( vDirection, vAim );

			AnglesNormalize( vAim );

			if( Config::Current->Aimbot->RCS )
			{
				auto punch = m_pLocal->m_vecPunchAngle();

				if( m_pLocal->m_iShotsFired() >= Config::Current->Aimbot->RCSDelay )
				{
					if( Config::Current->Aimbot->RCSAmountX )
						vAim.x -= punch.x * ( ( float )Config::Current->Aimbot->RCSAmountX / 50.0f );

					if( Config::Current->Aimbot->RCSAmountY )
						vAim.y -= punch.y * ( ( float )Config::Current->Aimbot->RCSAmountY / 50.0f );
				}
			}

			if( Config::Current->Aimbot->Smooth == 1 ) // Step
				ApplyStepSmooth( vAim );
			else if( Config::Current->Aimbot->Smooth == 2 ) // Linear
				ApplyLinearSmooth( vAim );

			vAim.z = 0.0f;

			AnglesNormalize( vAim );

			if( Config::Misc->Restriction == 2 )
			{
				auto aim = vAim;

				aim += m_pLocal->m_vecPunchAngle();

				POINT point = { 0, 0 };
				ScreenToClient( Source::m_pTargetInput->GetTarget(), &point );

				Vector3 forward;
				AngleVectors( aim, &forward );

				Vector3 end = forward * m_vTarget.DistTo( m_pLocal->EyePosition() ) + m_pLocal->EyePosition();
				Vector3 screen;
				if( Source::WorldToScreen( end, screen ) )
					SetCursorPos( ( int )screen.x - point.x, ( int )screen.y - point.y );
			}
			else
			{
				VectorCopy( vAim, m_pCmd->viewangles );

				if( !Config::Current->Aimbot->Silent )
					Source::m_pEngine->SetViewAngles( m_pCmd->viewangles );
			}

			if( Config::Current->Aimbot->AutoFire )
			{
				m_pCmd->buttons &= ~IN_RELOAD;
				m_pCmd->buttons |= IN_ATTACK;
			}

			if( Config::Current->Aimbot->AutoStop )
			{
				m_pCmd->forwardmove = 0.0f;
				m_pCmd->sidemove = 0.0f;
				m_pCmd->upmove = 0.0f;
			}

			if( Config::Current->Aimbot->AutoCrouch )
				m_pCmd->buttons |= IN_DUCK;
		}
		else
		{
			if( !Config::Current->Aimbot->NoSwitch )
			{
				if( Config::Current->Aimbot->SwitchDelay )
				{
					if( m_Timer.Elapsed() < Config::Current->Aimbot->SwitchDelay )
						return;
				}

				m_bChangeTarget = true;
			}
		}
	}

	void Aimbot::OnKeyEvent( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if( Config::Current->Aimbot->Mode != 2 )
			return;

		if( Config::Current->Aimbot->Key == 1 ) // Mouse 1
		{
			if( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK )
				m_bKeyPressed = true;
			else if( message == WM_LBUTTONUP )
				m_bKeyPressed = false;
		}
		else if( Config::Current->Aimbot->Key == 2 ) // Mouse 2
		{
			if( message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK )
				m_bKeyPressed = true;
			else if( message == WM_RBUTTONUP )
				m_bKeyPressed = false;
		}
		else if( Config::Current->Aimbot->Key == 4 ) // Mouse 3
		{
			if( message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK )
				m_bKeyPressed = true;
			else if( message == WM_MBUTTONUP )
				m_bKeyPressed = false;
		}
		else if( Config::Current->Aimbot->Key == 5 ) // Mouse 4
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
		else if( Config::Current->Aimbot->Key == 6 ) // Mouse 5
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
			if( wParam == Config::Current->Aimbot->Key )
			{
				if( message == WM_KEYDOWN || message == WM_SYSKEYDOWN )
					m_bKeyPressed = true;
				else if( message == WM_KEYUP || message == WM_SYSKEYUP )
					m_bKeyPressed = false;
			}
		}
	}

	void Aimbot::ChangeTarget()
	{
		int iMax = Source::m_pEngine->GetMaxClients();

		float flMax = m_pData->m_flRange;

		if( Config::Current->Aimbot->TargetSelection == 2 ) // Crosshair
			flMax = Config::Current->Aimbot->FieldOfView;

		int iSpawnTime = 0;

		for( int i = 1; i <= iMax; i++ )
		{
			auto pTarget = ToCSPlayer( Source::m_pEntList->GetBaseEntity( i ) );

			if( IsTargetGood( pTarget ) )
			{
				if( Config::Current->Aimbot->TargetSelection == 0 )
				{
					m_bChangeTarget = false;
					m_pTarget = pTarget;
					break;
				}
				else if( Config::Current->Aimbot->TargetSelection == 1 )
				{
					float flDistance = m_vTarget.DistTo( m_pLocal->EyePosition() );

					if( flDistance < flMax )
					{
						flMax = flDistance;

						m_bChangeTarget = false;
						m_pTarget = pTarget;
					}
				}
				else if( Config::Current->Aimbot->TargetSelection == 2 )
				{
					float flFOV = GetFOV( m_pCmd->viewangles + m_pLocal->m_vecPunchAngle() * 2.0f, m_pLocal->EyePosition(), m_vTarget );

					if( flFOV < flMax )
					{
						flMax = flFOV;

						m_bChangeTarget = false;
						m_pTarget = pTarget;
					}
				}
				else if( Config::Current->Aimbot->TargetSelection == 3 )
				{
					auto player = Source::m_pPlayerList->GetPlayer( i );

					if( player )
					{
						if( player->m_spawn_time >= iSpawnTime )
						{
							iSpawnTime = player->m_spawn_time;

							m_bChangeTarget = false;
							m_pTarget = pTarget;
						}
					}
				}
			}
		}
	}

	bool Aimbot::IsTargetGood( C_CSPlayer* pTarget )
	{
		if( !pTarget )
			return false;

		if( pTarget == m_pLocal )
			return false;

		if( pTarget->m_lifeState() != LIFE_ALIVE )
			return false;

		if( pTarget->IsDormant() )
			return false;

		if( Config::Current->Aimbot->Target == 1 ) // Enemy
		{
			if( pTarget->m_iTeamNum() == m_pLocal->m_iTeamNum() )
				return false;
		}
		else if( Config::Current->Aimbot->Target == 2 ) // Friendly
		{
			if( pTarget->m_iTeamNum() != m_pLocal->m_iTeamNum() )
				return false;
		}

		if( !pTarget->GetHitboxVector( Config::Current->Aimbot->Spot, m_vTarget ) )
			return false;

		if( m_vTarget.DistTo( m_pLocal->EyePosition() ) > m_pData->m_flRange )
			return false;

		if( Config::Current->Aimbot->TargetSelection == 2 ) // Crosshair
		{
			if( GetFOV( m_pCmd->viewangles + m_pLocal->m_vecPunchAngle() * 2.0f, m_pLocal->EyePosition(), m_vTarget ) > Config::Current->Aimbot->FieldOfView )
				return false;
		}

		if( Config::Current->Aimbot->AutoWall )
		{
			if( Config::Current->Aimbot->HitScan == 1 )
			{
				if( !Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), m_vTarget, Config::Current->Aimbot->MinDamage ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pTarget->GetModel() )->GetHitboxSet( pTarget->m_nHitboxSet() );

					int iBestDamage = Config::Current->Aimbot->MinDamage;

					bool bSuccess = false;

					for( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vCurrent;

						if( !pTarget->GetHitboxVector( i, vCurrent ) )
							continue;

						int iDamage = -1;

						Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), vCurrent, Config::Current->Aimbot->MinDamage, Config::Current->Aimbot->Target, &iDamage );

						if( iDamage >= iBestDamage )
						{
							iBestDamage = iDamage;
							m_vTarget = vCurrent;
							bSuccess = true;
						}
					}

					if( !bSuccess )
						return false;
				}
			}
			else if( Config::Current->Aimbot->HitScan == 2 )
			{
				if( !Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), m_vTarget, Config::Current->Aimbot->MinDamage ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pTarget->GetModel() )->GetHitboxSet( pTarget->m_nHitboxSet() );

					int iBestDamage = Config::Current->Aimbot->MinDamage;

					bool bSuccess = false;

					for( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vMin, vMax;

						if( !pTarget->GetHitboxBounds( i, vMin, vMax ) )
							continue;

						vMin *= Config::Current->Aimbot->HitScanScale;
						vMax *= Config::Current->Aimbot->HitScanScale;

						int iDamage = -1;

						if( Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), vMin, Config::Current->Aimbot->MinDamage, Config::Current->Aimbot->Target, &iDamage ) )
						{
							if( iDamage >= iBestDamage )
							{
								iBestDamage = iDamage;
								m_vTarget = vMin;
								bSuccess = true;
							}
						}
						else if( Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), vMax, Config::Current->Aimbot->MinDamage, Config::Current->Aimbot->Target, &iDamage ) )
						{
							if( iDamage >= iBestDamage )
							{
								iBestDamage = iDamage;
								m_vTarget = vMax;
								bSuccess = true;
							}
						}
					}

					if( !bSuccess )
						return false;
				}
			}
			else
			{
				if( !Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), m_vTarget, Config::Current->Aimbot->MinDamage ) )
					return false;
			}
		}
		else
		{
			if( Config::Current->Aimbot->HitScan == 1 )
			{
				if( !Source::TraceLine( m_vTarget, pTarget ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pTarget->GetModel() )->GetHitboxSet( pTarget->m_nHitboxSet() );

					bool bSuccess = false;

					for( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vCurrent;

						if( !pTarget->GetHitboxVector( i, vCurrent ) )
							continue;

						if( !Source::TraceLine( vCurrent ) )
							continue;

						bSuccess = true;

						m_vTarget = vCurrent;
					}

					if( !bSuccess )
						return false;
				}
			}
			else if( Config::Current->Aimbot->HitScan == 2 )
			{
				if( !Source::TraceLine( m_vTarget, pTarget ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pTarget->GetModel() )->GetHitboxSet( pTarget->m_nHitboxSet() );

					bool bSuccess = false;

					for( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vMin, vMax;

						if( !pTarget->GetHitboxBounds( i, vMin, vMax ) )
							continue;

						vMin *= Config::Current->Aimbot->HitScanScale;
						vMax *= Config::Current->Aimbot->HitScanScale;

						if( Source::TraceLine( vMin ) )
						{
							bSuccess = true;

							m_vTarget = vMin;
						}
						else if( Source::TraceLine( vMax ) )
						{
							bSuccess = true;

							m_vTarget = vMax;
						}
					}

					if( !bSuccess )
						return false;
				}
			}
			else
			{
				if( !Source::TraceLine( m_vTarget, pTarget ) )
					return false;
			}
		}

		return true;
	}

	void Aimbot::ApplyStepSmooth( Vector3& vAim )
	{
		Vector3 vDelta = vAim - m_pCmd->viewangles;

		AnglesNormalize( vDelta );

		if( Config::Current->Aimbot->StepX > 0.0f )
		{
			float flFactorX = ( Config::Current->Aimbot->StepX / 100.0f );

			if( vDelta.x < 0.0f )
			{
				if( flFactorX > fabs( vDelta.x ) )
					flFactorX = fabs( vDelta.x );

				vAim.x = m_pCmd->viewangles.x - flFactorX;
			}
			else
			{
				if( flFactorX > vDelta.x )
					flFactorX = vDelta.x;

				vAim.x = m_pCmd->viewangles.x + flFactorX;
			}
		}

		if( Config::Current->Aimbot->StepY > 0.0f )
		{
			float flFactorY = ( Config::Current->Aimbot->StepY / 100.0f );

			if( vDelta.y < 0.0f )
			{
				if( flFactorY > fabs( vDelta.y ) )
					flFactorY = fabs( vDelta.y );

				vAim.y = m_pCmd->viewangles.y - flFactorY;
			}
			else
			{
				if( flFactorY > vDelta.y )
					flFactorY = vDelta.y;

				vAim.y = m_pCmd->viewangles.y + flFactorY;
			}
		}
	}

	void Aimbot::ApplyLinearSmooth( Vector3& vAim )
	{
		Vector3 vDelta = m_pCmd->viewangles - vAim;

		AnglesNormalize( vDelta );

		float flFactorX = Config::Current->Aimbot->SmoothX;
		float flFactorY = Config::Current->Aimbot->SmoothY;

		if( flFactorX > 0.0f )
			vAim.x = m_pCmd->viewangles.x - vDelta.x / flFactorX;

		if( flFactorY > 0.0f )
			vAim.y = m_pCmd->viewangles.y - vDelta.y / flFactorY;
	}

	Vector3 Aimbot::MakeVector( const Vector3& angles )
	{
		float pitch = ToRadians( angles.x );
		float yaw = ToRadians( angles.y );
		float temp = cos( pitch );

		return Vector3( -temp * -cos( yaw ), sin( yaw ) * temp, -sin( pitch ) );
	}

	float Aimbot::GetFOV( const Vector3& va, const Vector3& src, const Vector3& dest )
	{
		auto dir = dest - src;

		VectorNormalize( dir );
		VectorAngles( dir, dir );

		auto aim = MakeVector( va );
		auto ang = MakeVector( dir );

		return ToDegrees( acos( aim.Dot( ang ) ) / aim.LengthSqr() );
	}
}