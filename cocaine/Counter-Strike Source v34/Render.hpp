#pragma once

#include "Valve.hpp"

namespace Feature
{
	using Direct3D9::Color;

	class Render
	{
	public:
		Render();

		void		OnDrawModel( void* ecx, ModelRenderInfo_t* info );

		IMaterial*	CreateMaterial( bool bVertexLit, bool bIgnoreZ );
		void		ForceMaterial( const Color& color, IMaterial* mat, bool mod = true );

	private:
		IMaterial*	m_pVertexIn;
		IMaterial*	m_pVertexOut;

		IMaterial*	m_pMatIn;
		IMaterial*	m_pMatOut;
	};
}