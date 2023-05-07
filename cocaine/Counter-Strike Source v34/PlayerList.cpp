#include "PlayerList.hpp"
#include "Source.hpp"
#include "Player.hpp"

namespace Feature
{
	Player::Player()
		: m_player( nullptr )
		, m_index( 0 )
		, m_pitch( 0 )
		, m_yaw( 0 )
		, m_spawn_time( 0 )
	{
		
	}

	void Player::Update( int index )
	{
		m_index = index;

		m_player = ToCSPlayer( Source::m_pEntList->GetBaseEntity( m_index ) );

		if( !m_player )
		{
			m_spawn_time = 0;
			return;
		}

		player_info_t data;
		if( Source::m_pEngine->GetPlayerInfo( m_index, &data ) )
			strcpy_s( m_name, sizeof( m_name ), data.name );

		if( m_player->m_lifeState() != LIFE_ALIVE )
		{
			m_spawn_time = 0;
			return;
		}

		m_spawn_time++;
	}

	void PlayerList::OnCreateMove()
	{
		int size = Source::m_pEngine->GetMaxClients();

		for( int i = 0; i <= size; i++ )
			m_players[ i ].Update( i );
	}

	Player* PlayerList::GetPlayer( int index )
	{
		return &m_players[ index ];
	}

	Player* PlayerList::GetPlayer( const char* name )
	{
		for( int i = 0; i <= 64; i++ )
		{
			if( strcmp( m_players[ i ].m_name, name ) == 0 )
				return &m_players[ i ];
		}

		return nullptr;
	}
}