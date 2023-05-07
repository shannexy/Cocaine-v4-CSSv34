#include "Player.hpp"
#include "Source.hpp"

C_CSPlayer* C_CSPlayer::GetLocalPlayer()
{
	auto iLocal = Source::m_pEngine->GetLocalPlayer();

	if( !iLocal )
		return nullptr;

	return ToCSPlayer( Source::m_pEntList->GetBaseEntity( iLocal ) );
}

// DT_BaseCombatCharacter
CBaseHandle& C_CSPlayer::m_hActiveWeapon()
{
	MakeNetVar( CBaseHandle, XorStr( "DT_BaseCombatCharacter" ), XorStr( "m_hActiveWeapon" ) );
}

float& C_CSPlayer::m_flNextAttack()
{
	MakeNetVar( float, XorStr( "DT_BaseCombatCharacter" ), XorStr( "m_flNextAttack" ) );
}

// DT_BaseAnimating
int& C_CSPlayer::m_nHitboxSet()
{
	MakeNetVar( int, XorStr( "DT_BaseAnimating" ), XorStr( "m_nHitboxSet" ) );
}

// DT_BasePlayer
Vector3& C_CSPlayer::m_vecPunchAngle()
{
	MakeNetVar( Vector3, XorStr( "DT_BasePlayer" ), XorStr( "m_vecPunchAngle" ) );
}

Vector3& C_CSPlayer::m_vecViewOffset()
{
	MakeNetVar( Vector3, XorStr( "DT_BasePlayer" ), XorStr( "m_vecViewOffset[0]" ) );
}

float& C_CSPlayer::m_flFallVelocity()
{
	MakeNetVar( float, XorStr( "DT_BasePlayer" ), XorStr( "m_flFallVelocity" ) );
}

Vector3& C_CSPlayer::m_vecVelocity()
{
	MakeNetVar( Vector3, XorStr( "DT_BasePlayer" ), XorStr( "m_vecVelocity[0]" ) );
}

Vector3& C_CSPlayer::m_vecBaseVelocity()
{
	MakeNetVar( Vector3, XorStr( "DT_BasePlayer" ), XorStr( "m_vecBaseVelocity" ) );
}

char& C_CSPlayer::m_lifeState()
{
	MakeNetVar( char, XorStr( "DT_BasePlayer" ), XorStr( "m_lifeState" ) );
}

int& C_CSPlayer::m_nTickBase()
{
	MakeNetVar( int, XorStr( "DT_BasePlayer" ), XorStr( "m_nTickBase" ) );
}

int& C_CSPlayer::m_iHealth()
{
	MakeNetVar( int, XorStr( "DT_BasePlayer" ), XorStr( "m_iHealth" ) );
}

int& C_CSPlayer::m_fFlags()
{
	MakeNetVar( int, XorStr( "DT_BasePlayer" ), XorStr( "m_fFlags" ) );
}

int& C_CSPlayer::m_iFOV()
{
	MakeNetVar( int, XorStr( "DT_BasePlayer" ), XorStr( "m_iFOV" ) );
}

// DT_CSPlayer
Vector3& C_CSPlayer::m_angEyeAngles()
{
	MakeNetVar( Vector3, XorStr( "DT_CSPlayer" ), XorStr( "m_angEyeAngles[0]" ) );
}

bool& C_CSPlayer::m_bIsDefusing()
{
	MakeNetVar( bool, XorStr( "DT_CSPlayer" ), XorStr( "m_bIsDefusing" ) );
}

bool& C_CSPlayer::m_bHasHelmet()
{
	MakeNetVar( bool, XorStr( "DT_CSPlayer" ), XorStr( "m_bHasHelmet" ) );
}

int& C_CSPlayer::m_iShotsFired()
{
	MakeNetVar( int, XorStr( "DT_CSPlayer" ), XorStr( "m_iShotsFired" ) );
}

int& C_CSPlayer::m_ArmorValue()
{
	MakeNetVar( int, XorStr( "DT_CSPlayer" ), XorStr( "m_ArmorValue" ) );
}

float& C_CSPlayer::m_flStamina()
{
	MakeNetVar( float, XorStr( "DT_CSPlayer" ), XorStr( "m_flStamina" ) );
}

float& C_CSPlayer::m_flFlashMaxAlpha()
{
	MakeNetVar( float, XorStr( "DT_CSPlayer" ), XorStr( "m_flFlashMaxAlpha" ) );
}

float& C_CSPlayer::m_flFlashDuration()
{
	MakeNetVar( float, XorStr( "DT_CSPlayer" ), XorStr( "m_flFlashDuration" ) );
}

void C_CSPlayer::SetCurrentCommand( CUserCmd* pCmd )
{
	*( CUserCmd** )( this + 0xD60 ) = pCmd;
}

// Custom
C_WeaponCSBaseGun* C_CSPlayer::GetActiveWeapon()
{
	return ( C_WeaponCSBaseGun* )Source::m_pEntList->GetBaseEntityFromHandle( m_hActiveWeapon() );
}

Vector3 C_CSPlayer::EyePosition()
{
	return ( m_vecOrigin() + m_vecViewOffset() );
}

Vector3 C_CSPlayer::EyeAngles()
{
	return ( m_angEyeAngles() );
}

bool C_CSPlayer::GetBoneCache( Matrix3x4* out )
{
	return SetupBones( out, 128, 256, Source::m_pGlobalVars->curtime );
}

bool C_CSPlayer::GetBoneVector( int bone, Vector3& out )
{
	Matrix3x4 mat[ 128 ];

	if( !GetBoneCache( mat ) )
		return false;

	out.x = mat[ bone ][ 0 ][ 3 ];
	out.y = mat[ bone ][ 1 ][ 3 ];
	out.z = mat[ bone ][ 2 ][ 3 ];

	return !( out.IsZero() );
}

bool C_CSPlayer::GetHitboxVector( int hitbox, Vector3& out )
{
	Matrix3x4 mat[ 128 ];

	if( !GetBoneCache( mat ) )
		return false;

	auto model = GetModel();

	if( !model )
		return false;

	auto studio = Source::m_pModelInfoClient->GetStudioModel( model );

	if( !studio )
		return false;

	auto set = studio->GetHitboxSet( m_nHitboxSet() );

	if( !set )
		return false;

	auto box = set->GetHitbox( hitbox );

	if( !box )
		return false;

	Vector3 mins, maxs;

	VectorTransform( box->bbmin, mat[ box->bone ], mins );
	VectorTransform( box->bbmax, mat[ box->bone ], maxs );

	if( mins.IsZero() || maxs.IsZero() )
		return false;

	out = ( mins + maxs ) * 0.5f;

	return !( out.IsZero() );
}

bool C_CSPlayer::GetHitboxBounds( int hitbox, Vector3& mins, Vector3& maxs )
{
	Matrix3x4 mat[ 128 ];

	if( !GetBoneCache( mat ) )
		return false;

	auto model = GetModel();

	if( !model )
		return false;

	auto studio = Source::m_pModelInfoClient->GetStudioModel( model );

	if( !studio )
		return false;

	auto set = studio->GetHitboxSet( m_nHitboxSet() );

	if( !set )
		return false;

	auto box = set->GetHitbox( hitbox );

	if( !box )
		return false;

	VectorTransform( box->bbmin, mat[ box->bone ], mins );
	VectorTransform( box->bbmax, mat[ box->bone ], maxs );

	return !( mins.IsZero() && maxs.IsZero() );
}