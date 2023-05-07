#include "Accuracy.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"

namespace Feature
{
	void Accuracy::ApplySpreadFix( C_WeaponCSBaseGun* weapon, int random_seed, const Vector3& input, Vector3& va, int type, bool inverted )
	{
		if( !type )
			return;

		auto spread = weapon->GetSpread();

		if( inverted )
			spread = -spread;

		Valve::RandomSeed( ( random_seed & 255 ) + 1 );

		float random[ 2 ] =
		{
			Valve::RandomFloat( -0.5f, 0.5f ) + Valve::RandomFloat( -0.5f, 0.5f ),
			Valve::RandomFloat( -0.5f, 0.5f ) + Valve::RandomFloat( -0.5f, 0.5f ),
		};

		Vector3 forward, right, up;

		if( type == 1 ) // Normal
		{
			AngleVectors( input, &forward, &right, &up );

			Vector3 direction = forward + ( -spread * random[ 0 ] * right ) + ( -spread * random[ 1 ] * up );
			VectorNormalize( direction );

			Vector3 compensated;
			VectorAngles( direction, compensated );

			va = compensated;
		}
		else if( type == 2 ) // Advanced
		{
			AngleVectors( vec3_zero, &forward, &right, &up );

			Vector3 direction = forward + ( -spread * random[ 0 ] * right ) + ( -spread * random[ 1 ] * up );
			VectorNormalize( direction );

			auto pitch = ToDegrees( std::atan2( direction.z, direction.x ) );

			if( pitch < 0.0f )
				pitch += 360.0f;

			pitch = -AngleNormalize( pitch );
			
			auto rotated_yaw = ( std::cos( ToRadians( input.x ) ) / direction.y );

			if( rotated_yaw > 1.0f || rotated_yaw < -1.0f )
				rotated_yaw = 1.0f / rotated_yaw;

			float yaw = ToDegrees( std::atan2( rotated_yaw, std::sqrt( 1.0f - square( rotated_yaw ) ) ) );

			if( yaw < 0.0f )
				yaw += 360.0f;

			yaw = AngleNormalize( yaw );

			va.x += pitch;
			va.y += yaw;
		}
	}

	void Accuracy::ApplyRecoilFix( C_CSPlayer* player, Vector3& va, bool inverted )
	{
		if( inverted )
			va += player->m_vecPunchAngle() * 2.0f;
		else
			va -= player->m_vecPunchAngle() * 2.0f;
	}

	bool Accuracy::CanPenetrate( const Vector3& vStart, const Vector3& vEnd, int iMinDamage, int iTeam, int* pDamageOut, int* pHitbox, int* pHitgroup, C_BaseEntity** ppEnt )
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer( );

		if( !pLocal )
			return false;

		auto pWeapon = pLocal->GetActiveWeapon( );

		if( !pWeapon )
			return false;

		auto pData = pWeapon->GetCSWpnData( );

		if( !pData )
			return false;

		trace_t tr;
		Ray_t ray;

		float flDistance = pData->m_flRange;
		int iPenetration = pData->m_iPenetration;
		int iBulletType = pWeapon->m_iPrimaryAmmoType( );
		int iDamage = pData->m_iDamage;
		float flRangeModifier = pData->m_flRangeModifier;

		float flCurrentDamage = ( float )iDamage;
		float flCurrentDistance = 0.0f;

		float flPenetrationPower = 0.0f;
		float flPenetrationDistance = 0.0f;
		float flDamageModifier = 0.5f;
		float flPenetrationModifier = 1.0f;

		Valve::GetBulletTypeParameters( iBulletType, flPenetrationPower, flPenetrationDistance );

		Vector3 vSource = vStart;

		Vector3 vDirection = vEnd - vStart;

		VectorNormalize( vDirection );

		C_CSPlayer* pLastPlayerHit = nullptr;

		int iModDamage = 0;

		while( flCurrentDamage > 0.0f )
		{
			Vector3 vDesired = vSource + vDirection * flDistance;

			CTraceFilterSkipTwoEntities trace( pLocal, pLastPlayerHit );

			ray.Set( vSource, vDesired );

			Source::m_pEngineTrace->TraceRay( ray, 0x4600400B, &trace, &tr );

			Valve::ClipTraceToPlayers( vSource, vDesired + vDirection * 40.0f, 0x4600400B, &trace, &tr );

			pLastPlayerHit = ToCSPlayer( tr.m_pEnt );

			if( tr.fraction == 1.0f )
				break;

			surfacedata_t* pSurfaceData = Source::m_pPhysicsSurfaceProps->GetSurfaceData( tr.surface.surfaceProps );

			int iEnterMaterial = pSurfaceData->game.material;

			Valve::GetMaterialParameters( iEnterMaterial, flPenetrationModifier, flDamageModifier );

			bool bHitGrate = ( tr.contents & 0x8 );

			if( bHitGrate )
			{
				flPenetrationModifier = 1.0f;
				flDamageModifier = 0.99f;
			}

			flCurrentDistance += tr.fraction * flDistance;
			flCurrentDamage *= pow( flRangeModifier, flCurrentDistance / 500.0f );

			if( flCurrentDistance > flPenetrationDistance && iPenetration > 0 )
				iPenetration = 0;

			if( pLastPlayerHit )
			{
				if( ( iTeam == 0 )
				||	( iTeam == 1 && pLocal->m_iTeamNum() != pLastPlayerHit->m_iTeamNum() )
				||	( iTeam == 2 && pLocal->m_iTeamNum() == pLastPlayerHit->m_iTeamNum() ) )
				{
					float flModDamage = Valve::GetHitgroupModDamage( flCurrentDamage, tr.hitgroup );

					iModDamage += Valve::GetPlayerModDamage( flModDamage, pLastPlayerHit->m_ArmorValue( ), pData->m_flArmorRatio, tr.hitgroup == 1, pLastPlayerHit->m_iTeamNum( ) == pLocal->m_iTeamNum( ), pLastPlayerHit->m_bHasHelmet( ) );

					if( pHitbox )
						*pHitbox = tr.hitbox;

					if( pHitgroup )
						*pHitgroup = tr.hitgroup;

					if( ppEnt )
						*ppEnt = pLastPlayerHit;
				}
			}

			if( iPenetration == 0 && !bHitGrate )
				break;

			if( iPenetration < 0 )
				break;

			Vector3 vPenetrationEnd;

			if( !Valve::TraceToExit( tr.endpos, vDirection, vPenetrationEnd, 24, 128 ) )
				break;

			trace_t exit;

			ray.Set( vPenetrationEnd, tr.endpos );

			Source::m_pEngineTrace->TraceRay( ray, 0x4600400B, nullptr, &exit );

			if( exit.m_pEnt != tr.m_pEnt && exit.m_pEnt != nullptr )
			{
				CTraceFilterSimple trace( exit.m_pEnt );

				ray.Set( vPenetrationEnd, tr.endpos );

				Source::m_pEngineTrace->TraceRay( ray, 0x4600400B, &trace, &exit );
			}

			pSurfaceData = Source::m_pPhysicsSurfaceProps->GetSurfaceData( exit.surface.surfaceProps );

			int iExitMaterial = pSurfaceData->game.material;

			bHitGrate = bHitGrate && ( exit.contents & 0x8 );

			if( iEnterMaterial == iExitMaterial )
			{
				if( iExitMaterial == CHAR_TEX_WOOD ||
					iExitMaterial == CHAR_TEX_METAL )
				{
					flPenetrationModifier *= 2.0f;
				}
			}

			float flTraceDistance = exit.endpos.DistTo( tr.endpos );

			if( flTraceDistance > ( flPenetrationPower * flPenetrationModifier ) )
				break;

			flPenetrationPower -= flTraceDistance / flPenetrationModifier;
			flCurrentDistance += flTraceDistance;

			vSource = exit.endpos;

			flDistance = ( flDistance - flCurrentDistance ) * 0.5f;

			flCurrentDamage *= flDamageModifier;

			iPenetration--;
		}

		if( iModDamage == 0 )
			iModDamage = -1;

		if( pDamageOut )
			*pDamageOut = iModDamage;

		if( iModDamage >= iMinDamage )
			return true;

		return false;
	}
}