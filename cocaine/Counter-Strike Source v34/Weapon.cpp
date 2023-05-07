#include "Weapon.hpp"
#include "Source.hpp"
#include "Player.hpp"

// DT_BaseCombatWeapon
float& C_WeaponCSBaseGun::m_flNextPrimaryAttack()
{
	MakeNetVar( float, XorStr( "DT_BaseCombatWeapon" ), XorStr( "m_flNextPrimaryAttack" ) );
}

CBaseHandle& C_WeaponCSBaseGun::m_hOwner()
{
	MakeNetVar( CBaseHandle, XorStr( "DT_BaseCombatWeapon" ), XorStr( "m_hOwner" ) );
}

int& C_WeaponCSBaseGun::m_iClip1()
{
	MakeNetVar( int, XorStr( "DT_BaseCombatWeapon" ), XorStr( "m_iClip1" ) );
}

int& C_WeaponCSBaseGun::m_iPrimaryAmmoType()
{
	MakeNetVar( int, XorStr( "DT_BaseCombatWeapon" ), XorStr( "m_iPrimaryAmmoType" ) );
}

// DT_WeaponCSBase
bool& C_WeaponCSBaseGun::m_bBurstMode()
{
	MakeNetVar( bool, XorStr( "DT_WeaponCSBase" ), XorStr( "m_bBurstMode" ) );
}

// DT_WeaponCSBaseGun
int& C_WeaponCSBaseGun::m_iBurstShotsRemaining()
{
	MakeNetVar( int, XorStr( "DT_WeaponCSBaseGun" ), XorStr( "m_iBurstShotsRemaining" ) );
}

float& C_WeaponCSBaseGun::m_flAccuracy()
{
	return *( float* )( this + 0x760 );
}

// C_WeaponCSBaseGun
const char* C_WeaponCSBaseGun::GetName()
{
	typedef const char* ( __thiscall* GetNameFn )( void* );
	return Memory::VCall< GetNameFn >( this, 260 )( this );
}

CCSWeaponInfo* C_WeaponCSBaseGun::GetCSWpnData()
{
	auto pData = Valve::GetCSWpnData( this );

	if( !pData )
	{
		DPRINT( XorStr( "[C_WeaponCSBaseGun::GetCSWpnData] Can't get weapon data!" ) );
		return nullptr;
	}

	if( IsModeSpecial() )
	{
		auto WeaponID = GetWeaponID();

		if( WeaponID == WEAPON_M4A1 )
		{
			pData->m_flRangeModifier = 0.95f;
		}
		else if( WeaponID == WEAPON_GLOCK )
		{
			pData->m_iDamage = 18;
			pData->m_flRangeModifier = 0.9f;
		}
		else if( WeaponID == WEAPON_USP )
		{
			pData->m_iDamage = 30;
		}
	}

	return pData;
}

CSWeaponType C_WeaponCSBaseGun::GetWeaponType()
{
	auto pData = GetCSWpnData();

	if( !pData )
	{
		DPRINT( XorStr( "[C_WeaponCSBaseGun::GetWeaponType] Can't get weapon data!" ) );
		return WEAPONTYPE_UNKNOWN;
	}

	return pData->m_WeaponType;
}

CSWeaponID C_WeaponCSBaseGun::GetWeaponID()
{
	typedef CSWeaponID ( __thiscall* GetWeaponIDFn )( void* );
	return Memory::VCall< GetWeaponIDFn >( this, 297 )( this );
}

bool C_WeaponCSBaseGun::IsModeSpecial( void )
{
	CSWeaponID WeaponID = GetWeaponID();

	if( WeaponID == WEAPON_GLOCK )
	{
		MakeNetVar( bool, "DT_WeaponGlock", "m_bBurstMode" );
	}
	else if( WeaponID == WEAPON_FAMAS )
	{
		MakeNetVar( bool, "DT_WeaponFamas", "m_bBurstMode" );
	}
	else if( WeaponID == WEAPON_M4A1 )
	{
		MakeNetVar( bool, "DT_WeaponM4A1", "m_bSilencerOn" );
	}
	else if( WeaponID == WEAPON_USP )
	{
		MakeNetVar( bool, "DT_WeaponUSP", "m_bSilenecerOn" );
	}

	return false;
}

bool C_WeaponCSBaseGun::IsMelee()
{
	auto WeaponType = GetWeaponType();

	return ( WeaponType == WEAPONTYPE_KNIFE ||
		WeaponType == WEAPONTYPE_C4 ||
		WeaponType == WEAPONTYPE_GRENADE );
}

bool C_WeaponCSBaseGun::IsFireTime()
{
	auto pPlayer = ToCSPlayer( Source::m_pEntList->GetBaseEntityFromHandle( m_hOwner() ) );

	if( !pPlayer )
		return false;

	if( IsModeSpecial() )
	{
		auto WeaponID = GetWeaponID();

		if( WeaponID == WEAPON_GLOCK )
		{
			if( GetNextBurstShot() != 0.0f )
				return true;
		}
		else if( WeaponID == WEAPON_FAMAS )
		{
			if( GetNextBurstShot() != 0.0f && GetNextBurstShot() < Source::m_pGlobalVars->curtime )
				return true;
		}
	}

	return ( Source::m_pGlobalVars->curtime >= pPlayer->m_flNextAttack() &&
			 Source::m_pGlobalVars->curtime >= m_flNextPrimaryAttack() );
}

float C_WeaponCSBaseGun::GetNextBurstShot()
{
	auto WeaponID = GetWeaponID();

	if( WeaponID == WEAPON_GLOCK )
		return *( float* )( this + 0x780 );
	else if( WeaponID == WEAPON_FAMAS )
		return *( float* )( this + 0x784 );

	return 0.0f;
}

float C_WeaponCSBaseGun::GetFamasBurstSpread()
{
	return *( float* )( this + 0x78C );
}

float C_WeaponCSBaseGun::GetSpread()
{
	auto pOwner = ToCSPlayer( Source::m_pEntList->GetBaseEntityFromHandle( m_hOwner() ) );

	if( !pOwner )
		return 0.0f;

	auto pData = GetCSWpnData();

	if( !pData )
		return 0.0f;

	float flSpread = 0.0f;

	int iSpeed = ( int )pOwner->m_vecVelocity().Length2D();

	auto WeaponID = GetWeaponID();

	switch( WeaponID )
	{
		case WEAPON_NONE:
		{
			flSpread = 0.0f;
			break;
		}
		case WEAPON_AK47:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( 0.04f + ( 0.4f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 140 )
				flSpread = ( 0.04f + ( 0.07f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.0275f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_AUG:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( 0.035f + ( 0.4f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 140 )
				flSpread = ( 0.035f + ( 0.07f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.02f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_AWP:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = 0.85f;
			else if( iSpeed > 140 )
				flSpread = 0.25f;
			else if( iSpeed > 10 )
				flSpread = 0.10f;
			else if( pOwner->m_fFlags() & FL_DUCKING )
				flSpread = 0.0f;
			else
				flSpread = 0.001f;

			if( pOwner->m_iFOV() == 0 || pOwner->m_iFOV() == 90 )
				flSpread += 0.08f;
			break;
		}
		case WEAPON_C4:
		{
			flSpread = 0.0f;
			break;
		}
		case WEAPON_DEAGLE:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 1.5f ) * ( 1 - m_flAccuracy() ) );
			else if( iSpeed > 5 )
				flSpread = ( ( 0.25f ) * ( 1 - m_flAccuracy() ) );
			else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
				flSpread = ( ( 0.115f ) * ( 1 - m_flAccuracy() ) );
			else
				flSpread = ( ( 0.13f ) * ( 1 - m_flAccuracy() ) );
			break;
		}
		case WEAPON_ELITE:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 1.3f ) * ( 1 - m_flAccuracy() ) );
			else if( iSpeed > 5 )
				flSpread = ( ( 0.175f ) * ( 1 - m_flAccuracy() ) );
			else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
				flSpread = ( ( 0.08f ) * ( 1 - m_flAccuracy() ) );
			else
				flSpread = ( ( 0.1f ) * ( 1 - m_flAccuracy() ) );
			break;
		}
		case WEAPON_FAMAS:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( 0.03f + ( 0.3f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 140 )
				flSpread = ( 0.03f + ( 0.07f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.02f ) * ( m_flAccuracy() ) );

			if( IsModeSpecial() )
			{
				if( GetNextBurstShot() != 0.0f && GetNextBurstShot() < Source::m_pGlobalVars->curtime )
					flSpread = GetFamasBurstSpread();
			}
			else
			{
				flSpread += 0.01f;
			}
			break;
		}
		case WEAPON_FIVESEVEN:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 1.5f ) * ( 1 - m_flAccuracy() ) );
			else if( iSpeed > 5 )
				flSpread = ( ( 0.255f ) * ( 1 - m_flAccuracy() ) );
			else if( pOwner->m_fFlags() & FL_DUCKING )
				flSpread = ( ( 0.075f ) * ( 1 - m_flAccuracy() ) );
			else
				flSpread = ( ( 0.15f ) * ( 1 - m_flAccuracy() ) );
			break;
		}
		case WEAPON_FLASHBANG:
		{
			flSpread = 0.0f;
			break;
		}
		case WEAPON_G3SG1:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 0.45f ) * ( 1 - m_flAccuracy() ) );
			else if( iSpeed > 5 )
				flSpread = 0.15f;
			else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
				flSpread = ( ( 0.035f ) * ( 1 - m_flAccuracy() ) );
			else
				flSpread = ( ( 0.055f ) * ( 1 - m_flAccuracy() ) );

			if( pOwner->m_iFOV() == 0 || pOwner->m_iFOV() == 90 )
				flSpread += 0.025f;
			break;
		}
		case WEAPON_GALIL:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( 0.04f + ( 0.3f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 140 )
				flSpread = ( 0.04f + ( 0.07f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.0375f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_GLOCK:
		{
			if( IsModeSpecial() )
			{
				if( GetNextBurstShot() != 0.0f )
				{
					flSpread = 0.05f;
				}
				else
				{
					if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
						flSpread = ( ( 1.2f ) * ( 1 - m_flAccuracy() ) );
					else if( iSpeed > 5 )
						flSpread = ( ( 0.185f ) * ( 1 - m_flAccuracy() ) );
					else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
						flSpread = ( ( 0.095f ) * ( 1 - m_flAccuracy() ) );
					else
						flSpread = ( ( 0.3f ) * ( 1 - m_flAccuracy() ) );
				}
			}
			else
			{
				if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
					flSpread = ( ( 1.0f ) * ( 1 - m_flAccuracy() ) );
				else if( iSpeed > 5 )
					flSpread = ( ( 0.165f ) * ( 1 - m_flAccuracy() ) );
				else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
					flSpread = ( ( 0.075f ) * ( 1 - m_flAccuracy() ) );
				else
					flSpread = ( ( 0.1f ) * ( 1 - m_flAccuracy() ) );
			}
			break;
		}
		case WEAPON_HEGRENADE:
		{
			flSpread = 0.0f;
			break;
		}
		case WEAPON_KNIFE:
		{
			flSpread = 0.0f;
			break;
		}
		case WEAPON_M249:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( 0.045f + ( 0.5f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 140 )
				flSpread = ( 0.045f + ( 0.095f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.03f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_M3:
		{
			flSpread = 0.0675f;
			break;
		}
		case WEAPON_M4A1:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( 0.035f + ( 0.4f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 140 )
				flSpread = ( 0.035f + ( 0.07f ) * ( m_flAccuracy() ) );
			else
			{
				if( IsModeSpecial() )
					flSpread = ( ( 0.025f ) * ( m_flAccuracy() ) );
				else
					flSpread = ( ( 0.02f ) * ( m_flAccuracy() ) );
			}
			break;
		}
		case WEAPON_MAC10:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 0.375f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.03f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_MP5NAVY:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 0.2f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.04f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_P228:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 1.5f ) * ( 1 - m_flAccuracy() ) );
			else if( iSpeed > 5 )
				flSpread = ( ( 0.255f ) * ( 1 - m_flAccuracy() ) );
			else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
				flSpread = ( ( 0.075f ) * ( 1 - m_flAccuracy() ) );
			else
				flSpread = ( ( 0.15f ) * ( 1 - m_flAccuracy() ) );
			break;
		}
		case WEAPON_P90:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 0.3f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 170 )
				flSpread = ( ( 0.115f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.045f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_SCOUT:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = 0.2f;
			else if( iSpeed > 170 )
				flSpread = 0.075f;
			else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
				flSpread = 0.0f;
			else
				flSpread = 0.007f;

			if( pOwner->m_iFOV() == 0 || pOwner->m_iFOV() == 90 )
				flSpread += 0.025f;
			break;
		}
		case WEAPON_SG550:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 0.45f ) * ( 1 - m_flAccuracy() ) );
			else if( iSpeed > 5 )
				flSpread = 0.15f;
			else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
				flSpread = ( ( 0.04f ) * ( 1 - m_flAccuracy() ) );
			else
				flSpread = ( ( 0.05f ) * ( 1 - m_flAccuracy() ) );

			if( pOwner->m_iFOV() == 0 || pOwner->m_iFOV() == 90 )
				flSpread += 0.025f;
			break;
		}
		case WEAPON_SG552:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( 0.035f + ( 0.45f ) * ( m_flAccuracy() ) );
			else if( iSpeed > 140 )
				flSpread = ( 0.035f + ( 0.075f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.02f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_SMOKEGRENADE:
		{
			flSpread = 0.0f;
			break;
		}
		case WEAPON_TMP:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 0.25f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.03f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_UMP45:
		{
			if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
				flSpread = ( ( 0.24f ) * ( m_flAccuracy() ) );
			else
				flSpread = ( ( 0.04f ) * ( m_flAccuracy() ) );
			break;
		}
		case WEAPON_USP:
		{
			if( IsModeSpecial() )
			{
				if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
					flSpread = ( ( 1.3f ) * ( 1 - m_flAccuracy() ) );
				else if( iSpeed > 5 )
					flSpread = ( ( 0.25f ) * ( 1 - m_flAccuracy() ) );
				else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
					flSpread = ( ( 0.125f ) * ( 1 - m_flAccuracy() ) );
				else
					flSpread = ( ( 0.15f ) * ( 1 - m_flAccuracy() ) );
			}
			else
			{
				if( !( pOwner->m_fFlags() & FL_ONGROUND ) )
					flSpread = ( ( 1.2f ) * ( 1 - m_flAccuracy() ) );
				else if( iSpeed > 5 )
					flSpread = ( ( 0.225f ) * ( 1 - m_flAccuracy() ) );
				else if( ( pOwner->m_fFlags() & FL_DUCKING ) )
					flSpread = ( ( 0.08f ) * ( 1 - m_flAccuracy() ) );
				else
					flSpread = ( ( 0.1f ) * ( 1 - m_flAccuracy() ) );
			}
			break;
		}
		case WEAPON_XM1014:
		{
			flSpread = 0.0725f;
			break;
		}
		default:
		{
			flSpread = 0.0f;
			break;
		}
	}

	return flSpread;
}