#pragma once

#include "Valve.hpp"

#include "NetVarManager.hpp"

#include "Feature.hpp"

namespace Source
{
	extern IBaseClientDLL*							m_pClient;
	extern IClientEntityList*						m_pEntList;
	extern IGameMovement*							m_pGameMovement;
	extern IPrediction*								m_pPrediction;
	extern IVEngineClient*							m_pEngine;
	extern IVModelInfoClient*						m_pModelInfoClient;
	extern IVRenderView*							m_pRenderView;
	extern IVModelRender*							m_pModelRender;
	extern IEngineTrace*							m_pEngineTrace;
	extern ICvar*									m_pCvar;
	extern IPhysicsSurfaceProps*					m_pPhysicsSurfaceProps;
	extern IMaterialSystem*							m_pMatSystem;

	extern CGlobalVars*								m_pGlobalVars;
	extern IInput*									m_pInput;

	extern IDirect3DDevice9*						m_pDevice;

	extern std::shared_ptr< Input::Win32 >			m_pTargetInput;

	extern std::shared_ptr< Source::NetVarManager >	m_pNetVarManager;
	extern std::shared_ptr< Direct3D9::Renderer >	m_pRenderer;
	
	extern Direct3D9::HFont							m_hFont;

	extern std::shared_ptr< Feature::Accuracy >		m_pAccuracy;
	extern std::shared_ptr< Feature::Aimbot >		m_pAimbot;
	extern std::shared_ptr< Feature::Triggerbot >	m_pTriggerbot;
	extern std::shared_ptr< Feature::DataManager >	m_pDataManager;
	extern std::shared_ptr< Feature::PlayerList >	m_pPlayerList;
	extern std::shared_ptr< Feature::Menu >			m_pMenu;
	extern std::shared_ptr< Feature::Render >		m_pRender;

	extern std::shared_ptr< Memory::VmtSwap >		m_pClientSwap;
	extern std::shared_ptr< Memory::VmtSwap >		m_pPredictionSwap;
	extern std::shared_ptr< Memory::VmtSwap >		m_pInputSwap;
	extern std::shared_ptr< Memory::VmtSwap >		m_pModelRenderSwap;
	extern std::shared_ptr< Memory::VmtSwap >		m_pDeviceSwap;

	extern std::shared_ptr< Memory::Detour >		m_pPresentSwap;
	extern std::shared_ptr< Memory::Detour >		m_pRunPredictionSwap;

	extern bool										Startup();
	extern bool										Release();
	extern void										Free();

	extern void*									QueryInterface( const char* szMod, const char* szName, bool bCustom = false );
	extern void										MovementFix( CUserCmd* cmd, const Vector3& va, bool aa = false );
	extern bool										TraceLine( const Vector3& vEnd, C_BaseEntity* pEnt = nullptr );
	extern bool										WorldToScreen( const Vector3& vPoint, Vector3& vOut );

}