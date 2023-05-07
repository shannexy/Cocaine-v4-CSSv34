#include "Entity.hpp"
#include "Source.hpp"

// ICollideable
const Vector3& C_BaseEntity::GetMins()
{
	auto pCol = GetCollideable();

	typedef const Vector3& ( __thiscall* GetMinsFn )( void* );
	return Memory::VCall< GetMinsFn >( pCol, 1 )( pCol );
}

const Vector3& C_BaseEntity::GetMaxs()
{
	auto pCol = GetCollideable();

	typedef const Vector3& ( __thiscall* GetMaxsFn )( void* );
	return Memory::VCall< GetMaxsFn >( pCol, 2 )( pCol );
}

// IClientNetworkable
ClientClass* C_BaseEntity::GetClientClass()
{
	auto pNet = GetNetworkable();

	typedef ClientClass* ( __thiscall* GetClientClassFn )( void* );
	return Memory::VCall< GetClientClassFn >( pNet, 2 )( pNet );
}

bool C_BaseEntity::IsDormant()
{
	auto pNet = GetNetworkable();

	typedef bool ( __thiscall* IsDormantFn )( void* );
	return Memory::VCall< IsDormantFn >( pNet, 8 )( pNet );
}

int C_BaseEntity::GetIndex()
{
	auto pNet = GetNetworkable();

	typedef int ( __thiscall* GetIndexFn )( void* );
	return Memory::VCall< GetIndexFn >( pNet, 9 )( pNet );
}

// IClientRenderable
const model_t* C_BaseEntity::GetModel()
{
	auto pRen = GetRenderable();

	typedef const model_t* ( __thiscall* GetModelFn )( void* );
	return Memory::VCall< GetModelFn >( pRen, 8 )( pRen );
}

bool C_BaseEntity::SetupBones( Matrix3x4* pOut, int iMaxBones, int iBoneMask, float fCurrentTime )
{
	auto pRen = GetRenderable();

	typedef bool ( __thiscall* SetupBonesFn )( void*, Matrix3x4*, int, int, float );
	return Memory::VCall< SetupBonesFn >( pRen, 15 )( pRen, pOut, iMaxBones, iBoneMask, fCurrentTime );
}

// C_BaseEntity
bool C_BaseEntity::IsPlayer()
{
	typedef bool ( __thiscall* IsPlayerFn )( void* );
	return Memory::VCall< IsPlayerFn >( this, 118 )( this ); // search for CCSGameMovement::CheckParameters
}

unsigned char& C_BaseEntity::m_MoveType()
{
	return *( unsigned char* )( this + 0x134 ); // search for CCSGameMovement::CheckParameters
}

// DT_BaseEntity
int& C_BaseEntity::m_iTeamNum()
{
	MakeNetVar( int, XorStr( "DT_BaseEntity" ), XorStr( "m_iTeamNum" ) );
}

Vector3& C_BaseEntity::m_vecOrigin()
{
	MakeNetVar( Vector3, XorStr( "DT_BaseEntity" ), XorStr( "m_vecOrigin" ) );
}

Matrix3x4& C_BaseEntity::m_rgflCoordinateFrame()
{
	static auto var = Source::m_pNetVarManager->Get( XorStr( "DT_BasePlayer" ), XorStr( "m_fFlags" ) ) - sizeof( Vector3 ) - sizeof( Vector3 ) - sizeof( Matrix3x4 );

	return *( Matrix3x4* )( this + var );
}

// private
void* C_BaseEntity::GetCollideable()
{
	typedef void* ( __thiscall* GetCollideableFn )( void* );
	return Memory::VCall< GetCollideableFn >( this, 3 )( this );
}

void* C_BaseEntity::GetNetworkable()
{
	typedef void* ( __thiscall* GetNetworkableFn )( void* );
	return Memory::VCall< GetNetworkableFn >( this, 4 )( this );
}

void* C_BaseEntity::GetRenderable()
{
	typedef void* ( __thiscall* GetRenderableFn )( void* );
	return Memory::VCall< GetRenderableFn >( this, 5 )( this );
}