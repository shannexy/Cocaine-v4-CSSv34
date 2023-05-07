#pragma once

#include "Valve.hpp"

#define MakeNetVar( cast, table, name ) static auto var = Source::m_pNetVarManager->Get( table, name ); return *( cast* )( this + var )

class IHandleEntity
{
public:
	virtual ~IHandleEntity( void ) { }
	virtual void SetRefEHandle( const CBaseHandle& handle ) = 0;
	virtual const CBaseHandle& GetRefEHandle( void ) const = 0;
};

class C_BaseEntity
{
public:
	// ICollideable
	const Vector3&	GetMins();
	const Vector3&	GetMaxs();

	// IClientNetworkable
	ClientClass*	GetClientClass();
	bool			IsDormant();
	int				GetIndex();

	// IClientRenderable
	const model_t*	GetModel();
	bool			SetupBones( Matrix3x4* pOut, int iMaxBones, int iBoneMask, float fCurrentTime );

	// C_BaseEntity
	bool			IsPlayer();

	// DT_BaseEntity
	unsigned char&	m_MoveType();

	int&			m_iTeamNum();
	Vector3&		m_vecOrigin();
	Matrix3x4&		m_rgflCoordinateFrame();

public:
	// Source
	void*			GetCollideable();
	void*			GetNetworkable();
	void*			GetRenderable();
};