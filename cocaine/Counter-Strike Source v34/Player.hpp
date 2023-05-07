#pragma once

#include "Entity.hpp"

class C_CSPlayer : public C_BaseEntity
{
public:
	static C_CSPlayer*	GetLocalPlayer();

	// DT_BaseCombatCharacter
	CBaseHandle&		m_hActiveWeapon();
	float&				m_flNextAttack();

	// DT_BaseAnimating
	int&				m_nHitboxSet();

	// DT_BasePlayer
	Vector3&			m_vecPunchAngle();
	Vector3&			m_vecViewOffset();
	float&				m_flFallVelocity();
	Vector3&			m_vecVelocity();
	Vector3&			m_vecBaseVelocity();
	char&				m_lifeState();
	int&				m_nTickBase();
	int&				m_iHealth();
	int&				m_fFlags();
	int&				m_iFOV();

	// DT_CSPlayer
	Vector3&			m_angEyeAngles();

	bool&				m_bIsDefusing();
	bool&				m_bHasHelmet();

	int&				m_iShotsFired();
	int&				m_ArmorValue();

	float&				m_flStamina();
	float&				m_flFlashMaxAlpha();
	float&				m_flFlashDuration();

	void				SetCurrentCommand( CUserCmd* pCmd );

	// Custom
	C_WeaponCSBaseGun*	GetActiveWeapon();

	Vector3				EyePosition();
	Vector3				EyeAngles();

	bool				GetBoneCache( Matrix3x4* out );

	bool				GetBoneVector( int bone, Vector3& out );
	bool				GetHitboxVector( int hitbox, Vector3& out );
	bool				GetHitboxBounds( int hitbox, Vector3& mins, Vector3& maxs );
};

inline C_CSPlayer* ToCSPlayer( C_BaseEntity* pEnt )
{
	if( !pEnt )
		return nullptr;

	if( !pEnt->IsPlayer() )
		return nullptr;

	return ( C_CSPlayer* )pEnt;
}