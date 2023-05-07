#include "Render.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Config.hpp"

namespace Feature
{
	Render::Render()
	{
		m_pVertexIn = CreateMaterial( true, true );
		m_pVertexOut = CreateMaterial( true, false );
		m_pMatIn = CreateMaterial( false, true );
		m_pMatOut = CreateMaterial( false, false );
	}

	void Render::OnDrawModel( void* ecx, ModelRenderInfo_t* info )
	{
		auto player = C_CSPlayer::GetLocalPlayer();

		if( !player )
			return;

		auto enemy = ToCSPlayer( Source::m_pEntList->GetBaseEntity( info->ent_index ) );

		if( !enemy )
			return;

		if( enemy->m_lifeState() != LIFE_ALIVE )
			return;

		if( Config::Render->ChamsTarget == 1 ) // Enemy
		{
			if( player->m_iTeamNum() == enemy->m_iTeamNum() )
				return;
		}
		else if( Config::Render->ChamsTarget == 2 ) // Friendly
		{
			if( player->m_iTeamNum() != enemy->m_iTeamNum() )
				return;
		}

		Color color;

		if( enemy->m_iTeamNum() == 2 ) // T
			color = Config::Colors->T_Chams_Normal;
		else if( enemy->m_iTeamNum() == 3 ) // CT
			color = Config::Colors->CT_Chams_Normal;

		IMaterial* pIn = nullptr;
		IMaterial* pOut = nullptr;

		if( Config::Render->ChamsMode == 1 ) // Flat
		{
			pIn = m_pMatIn;
			pOut = m_pMatOut;
		}
		else if( Config::Render->ChamsMode == 2 ) // Shadow
		{
			pIn = m_pVertexIn;
			pOut = m_pVertexOut;
		}

		ForceMaterial( color, pIn );

		Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );

		if( Config::Render->ChamsColored )
		{
			if( enemy->m_iTeamNum() == 2 ) // T
				color = Config::Colors->T_Chams_Colored;
			else if( enemy->m_iTeamNum() == 3 ) // CT
				color = Config::Colors->CT_Chams_Colored;
		}

		ForceMaterial( color, pOut );
	}

	IMaterial* Render::CreateMaterial( bool bVertexLit, bool bIgnoreZ )
	{
		static int iCreated = 0;
		static const char szMaterialStruct[ ] =
		{
			"\"%s\"\
			\n{\
			\n\t\"$basetexture\" \"vgui/WHITE\"\
			\n\t\"$model\" \"1\"\
			\n\t\"$nocull\" \"1\"\
			\n\t\"$nofog\" \"1\"\
			\n\t\"$ignorez\" \"%i\"\
			\n\t\"$znearer\" \"1\"\
			\n\t\"$wireframe\" \"0\"\
			\n}\n"
		};

		const char* szBaseType = bVertexLit ? "VertexLitGeneric" : "UnlitGeneric";

		char szMaterial[ 512 ];
		sprintf_s( szMaterial, sizeof( szMaterial ), szMaterialStruct, szBaseType, bIgnoreZ ? 1 : 0 );

		char szName[ 512 ];
		sprintf_s( szName, sizeof( szName ), "custom_material_%i.vmt", iCreated );

		iCreated++;

		KeyValues* pKey = new KeyValues( szBaseType );

		pKey->LoadFromBuffer( szName, szMaterial );

		IMaterial* pMat = Source::m_pMatSystem->CreateMaterial( szName, pKey );

		return pMat;
	}

	void Render::ForceMaterial( const Color& color, IMaterial* mat, bool mod )
	{
		if( mod )
		{
			float col[ 3 ] =
			{
				color.R / 255.0f,
				color.G / 255.0f,
				color.B / 255.0f
			};

			float alpha = color.A / 255.0f;

			Source::m_pRenderView->SetBlend( alpha );
			Source::m_pRenderView->SetColorModulation( col );
		}

		Source::m_pModelRender->ForcedMaterialOverride( mat );
	}
}