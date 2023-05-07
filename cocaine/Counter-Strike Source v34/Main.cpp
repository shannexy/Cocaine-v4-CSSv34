#include "Source.hpp"
#include "Config.hpp"

#include "ImGui.hpp"

void Eject()
{
	if( !Source::Release() )
		DPRINT( XorStr( "[Eject] Can't release 'Source' hooks!" ) );

	Sleep( 1000 );
	Source::Free();
	Config::Release();
}

void Heart( HMODULE hMod )
{
	Config::Startup( hMod );

	if( !Source::Startup() )
	{
		DPRINT( XorStr( "[Startup] Can't initialize 'Source' hooks! Ejecting!" ) );
		FreeLibraryAndExitThread( hMod, EXIT_SUCCESS );
	}

	while( !Shared::m_bEject )
	{
		if( Shared::m_bLoad )
		{
			Config::Load( Shared::m_strConfig );
			ImGui::LoadSettings( std::string( Config::GetPath() + XorStr( "gui" ) ).c_str() );
			Shared::m_bLoad = false;
		}

		if( Shared::m_bSave )
		{
			Config::Save( Shared::m_strConfig );
			ImGui::SaveSettings( std::string( Config::GetPath() + XorStr( "gui" ) ).c_str() );
			Shared::m_bSave = false;
		}

		Sleep( 100 );
	}

	FreeLibraryAndExitThread( hMod, EXIT_SUCCESS );
}

void OnProcessAttach( HMODULE hMod, LPVOID lpReserved )
{
	DisableThreadLibraryCalls( hMod );
	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )Heart, ( LPVOID )hMod, NULL, NULL );
}

void OnProcessDetach()
{
	Eject();
}

BOOL WINAPI DllMain( HMODULE hMod, DWORD dwReason, LPVOID lpReserved )
{
	switch( dwReason )
	{
		case DLL_PROCESS_ATTACH:
		{
			OnProcessAttach( hMod, lpReserved );
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			OnProcessDetach();
			break;
		}
	}

	return TRUE;
}