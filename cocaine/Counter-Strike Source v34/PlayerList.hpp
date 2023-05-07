#pragma once

#include "Valve.hpp"

namespace Feature
{
	class Player
	{
	public:
		Player();

		void Update( int index );

	public:
		C_CSPlayer* m_player;

		int m_index;
		int m_pitch;
		int m_yaw;
		int m_spawn_time;
		
		char m_name[ 32 ];
	};

	class PlayerList
	{
	public:
		void OnCreateMove();

		Player* GetPlayer( int index );
		Player* GetPlayer( const char* name );

	private:
		Player m_players[ 65 ];
	};
}