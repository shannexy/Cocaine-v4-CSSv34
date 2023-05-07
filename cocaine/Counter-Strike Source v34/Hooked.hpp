#pragma once

#include "Valve.hpp"

void __fastcall Hooked_CreateMove( void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active );
void __fastcall Hooked_FrameStageNotify( void* ecx, void* edx, ClientFrameStage_t curStage );
void __fastcall Hooked_RunCommand( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper );
void __fastcall Hooked_FinishMove( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, CMoveData* move );
void __fastcall Hooked_Update( void* ecx, void* edx, bool recieved_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command );

CUserCmd* __fastcall Hooked_GetUserCmd( void* ecx, void* edx, int sequence_number );
void __fastcall Hooked_ResetMouse( void* ecx, void* edx );

int __fastcall Hooked_DrawModelEx( void* ecx, void* edx, ModelRenderInfo_t* info );

HRESULT D3DAPI Hooked_Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters );
void Hooked_Present();

void Hooked_CL_RunPrediction( PREDICTION_REASON reason );

void DT_BasePlayer_m_nTickBase( const CRecvProxyData* pData, void* pStruct, void* pOut );
void DT_BasePlayer_m_vecPunchAngle( const CRecvProxyData* pData, void* pStruct, void* pOut );
void DT_ParticleSmokeGrenade_m_flSpawnTime( const CRecvProxyData* pData, void* pStruct, void* pOut );
void DT_CSPlayer_m_angEyeAnglesX( const CRecvProxyData* pData, void* pStruct, void* pOut );
void DT_CSPlayer_m_angEyeAnglesY( const CRecvProxyData* pData, void* pStruct, void* pOut );