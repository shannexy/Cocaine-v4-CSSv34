#pragma once

#include "Entity.hpp"

class CCSWeaponInfo
{
public:
	MakePad( 0x654 );

	float			m_flMaxSpeed;
	CSWeaponType	m_WeaponType;

	MakePad( 0x008 );

	float			m_flArmorRatio;

	MakePad( 0x174 );

	int				m_iPenetration;
	int				m_iDamage;
	float			m_flRange;
	float			m_flRangeModifier;
	int				m_iBullets;
	float			m_flCycleTime;
};

class C_WeaponCSBaseGun : public C_BaseEntity
{
public:
	// DT_BaseCombatWeapon
	float&			m_flNextPrimaryAttack();
	CBaseHandle&	m_hOwner();
	int&			m_iClip1();
	int&			m_iPrimaryAmmoType();

	// DT_WeaponCSBase
	bool&			m_bBurstMode();

	// DT_WeaponCSBaseGun
	int&			m_iBurstShotsRemaining();

	float&			m_flAccuracy();

	// C_WeaponCSBaseGun
	const char*		GetName();
	CCSWeaponInfo*	GetCSWpnData();
	CSWeaponType	GetWeaponType();
	CSWeaponID		GetWeaponID();

	// Custom
	bool			IsModeSpecial();
	bool			IsMelee();
	bool			IsFireTime();

	float			GetNextBurstShot();
	float			GetFamasBurstSpread();
	float			GetSpread();
};