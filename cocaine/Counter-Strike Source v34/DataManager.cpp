#include "DataManager.hpp"
#include "Source.hpp"
#include "Player.hpp"

namespace Feature
{
	const Vector3& DataEntry::GetPunchAngle() const
	{
		return m_vecPunchAngle;
	}

	void DataEntry::SetPunchAngle( const Vector3& vecPunchAngle )
	{
		m_vecPunchAngle = vecPunchAngle;
	}

	DataManager::DataManager()
		:	m_flCurrentTime( 0.0f ),
			m_flFrameTime( 0.0f ),
			m_fFlags( 0 )
	{

	}

	void DataManager::PreCreateMove( CUserCmd* cmd, C_CSPlayer* player )
	{
		m_fFlags = player->m_fFlags();

		m_flCurrentTime = Source::m_pGlobalVars->curtime;
		m_flFrameTime = Source::m_pGlobalVars->frametime;

		player->SetCurrentCommand( cmd );

		Valve::SetPredictionSeed( cmd );
		Valve::SetPredictionPlayer( player );

		Source::m_pGlobalVars->curtime = ( float )player->m_nTickBase() * Source::m_pGlobalVars->interval_per_tick;
		Source::m_pGlobalVars->frametime = Source::m_pGlobalVars->interval_per_tick;

		CMoveData Move;
		ZeroMemory( &Move, sizeof( Move ) );

		Source::m_pPrediction->SetupMove( player, cmd, nullptr, &Move );
		Source::m_pGameMovement->ProcessMovement( player, &Move );
		Source::m_pPrediction->FinishMove( player, cmd, &Move );
	}

	void DataManager::PostCreateMove( C_CSPlayer* player )
	{
		player->SetCurrentCommand( nullptr );

		Valve::SetPredictionSeed( nullptr );
		Valve::SetPredictionPlayer( nullptr );

		Source::m_pGlobalVars->curtime = m_flCurrentTime;
		Source::m_pGlobalVars->frametime = m_flFrameTime;
	}

	void DataManager::OnDataRecieved()
	{
		auto player = C_CSPlayer::GetLocalPlayer();

		if( !player )
			return;

		int nTickBase = player->m_nTickBase();

		auto pEntry = &m_SavedData[ nTickBase % 128 ];

		if( !pEntry )
			return;

		player->m_vecPunchAngle() = pEntry->GetPunchAngle();
	}

	void DataManager::OnDataUpdate()
	{
		auto player = C_CSPlayer::GetLocalPlayer();

		if( !player )
			return;

		int nTickBase = player->m_nTickBase();

		auto pEntry = &m_SavedData[ nTickBase % 128 ];

		if( !pEntry )
			return;

		pEntry->SetPunchAngle( player->m_vecPunchAngle() );
	}

	int DataManager::GetFlags()
	{
		return m_fFlags;
	}
}