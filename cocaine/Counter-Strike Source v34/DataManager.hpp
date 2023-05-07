#pragma once

#include "Valve.hpp"

namespace Feature
{
	class DataEntry
	{
	public:
		const Vector3&	GetPunchAngle() const;
		void			SetPunchAngle( const Vector3& vecPunchAngle );

	private:
		Vector3			m_vecPunchAngle;
	};

	class DataManager
	{
	public:
		DataManager();

		void		PreCreateMove( CUserCmd* cmd, C_CSPlayer* player );
		void		PostCreateMove( C_CSPlayer* player );

		void		OnDataRecieved();
		void		OnDataUpdate();

		int			GetFlags();

	private:
		float		m_flCurrentTime;
		float		m_flFrameTime;

		int			m_fFlags;

		DataEntry	m_SavedData[ 128 ];
	};
}