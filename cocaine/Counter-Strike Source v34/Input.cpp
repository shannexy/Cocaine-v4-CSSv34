#include "Input.hpp"
#include "Source.hpp"
#include "Config.hpp"
#include "Aimbot.hpp"

HWND hTarget( nullptr );

BOOL WINAPI EnumWnd( HWND hWnd, LPARAM lParam )
{
	DWORD dwProcessId( NULL );

	GetWindowThreadProcessId( hWnd, &dwProcessId );

	if( dwProcessId == lParam )
	{
		hTarget = hWnd;
		return FALSE;
	}

	return TRUE;
}

namespace Input
{
	Win32::Win32()
		:	m_hTarget( nullptr ),
			m_pProcedure( nullptr )
	{

	}

	Win32::~Win32()
	{
		Release();
	}

	bool Win32::Capture()
	{
		EnumWindows( EnumWnd, GetCurrentProcessId() );

		m_hTarget = hTarget;

		if( !m_hTarget )
		{
			DPRINT( XorStr( "[Win32::Capture] Can't get target window!" ) );
			return false;
		}

		if( !( m_pProcedure = ( WNDPROC )SetWindowLongPtr( m_hTarget, GWL_WNDPROC, ( LONG_PTR )Proxy ) ) )
		{
			DPRINT( XorStr( "[Win32::Capture] Can't replace window procedure!" ) );
			return false;
		}

		return true;
	}

	bool Win32::Release()
	{
		if( !m_pProcedure )
		{
			DPRINT( XorStr( "[Win32::Release] Window procedure is not hooked!" ) );
			return true;
		}

		if( !SetWindowLongPtr( m_hTarget, GWL_WNDPROC, ( LONG_PTR )m_pProcedure ) )
		{
			DPRINT( XorStr( "[Win32::Release] Can't replace window procedure!" ) );
			return false;
		}

		m_hTarget = nullptr;
		m_pProcedure = nullptr;

		return true;
	}

	const HWND Win32::GetTarget() const
	{
		return m_hTarget;
	}

	void Speed( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if( Config::Misc->SpeedKey == 1 ) // Mouse 1
		{
			if( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK )
				Shared::m_bSpeed = true;
			else if( message == WM_LBUTTONUP )
				Shared::m_bSpeed = false;
		}
		else if( Config::Misc->SpeedKey == 2 ) // Mouse 2
		{
			if( message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK )
				Shared::m_bSpeed = true;
			else if( message == WM_RBUTTONUP )
				Shared::m_bSpeed = false;
		}
		else if( Config::Misc->SpeedKey == 4 ) // Mouse 3
		{
			if( message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK )
				Shared::m_bSpeed = true;
			else if( message == WM_MBUTTONUP )
				Shared::m_bSpeed = false;
		}
		else if( Config::Misc->SpeedKey == 5 ) // Mouse 4
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON1 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bSpeed = true;
				else if( message == WM_XBUTTONUP )
					Shared::m_bSpeed = false;
			}
		}
		else if( Config::Misc->SpeedKey == 6 ) // Mouse 5
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON2 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bSpeed = true;
				else if( message == WM_XBUTTONUP )
					Shared::m_bSpeed = false;
			}
		}
		else
		{
			if( wParam == Config::Misc->SpeedKey )
			{
				if( message == WM_KEYDOWN || message == WM_SYSKEYDOWN )
					Shared::m_bSpeed = true;
				else if( message == WM_KEYUP || message == WM_SYSKEYUP )
					Shared::m_bSpeed = false;
			}
		}
	}

	void AirStuck( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if( Config::Misc->StuckKey == 1 ) // Mouse 1
		{
			if( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK )
				Shared::m_bStuck = !Shared::m_bStuck;
		}
		else if( Config::Misc->StuckKey == 2 ) // Mouse 2
		{
			if( message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK )
				Shared::m_bStuck = !Shared::m_bStuck;
		}
		else if( Config::Misc->StuckKey == 4 ) // Mouse 3
		{
			if( message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK )
				Shared::m_bStuck = !Shared::m_bStuck;
		}
		else if( Config::Misc->StuckKey == 5 ) // Mouse 4
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON1 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bStuck = !Shared::m_bStuck;
			}
		}
		else if( Config::Misc->StuckKey == 6 ) // Mouse 5
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON2 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bStuck = !Shared::m_bStuck;
			}
		}
		else
		{
			if( wParam == Config::Misc->StuckKey )
			{
				if( message == WM_KEYDOWN || message == WM_SYSKEYDOWN )
					Shared::m_bStuck = !Shared::m_bStuck;
			}
		}
	}

	void Extra( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if( Config::Binds->Menu == 1 ) // Mouse 1
		{
			if( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK )
				Shared::m_bMenu = !Shared::m_bMenu;
		}
		else if( Config::Binds->Menu == 2 ) // Mouse 2
		{
			if( message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK )
				Shared::m_bMenu = !Shared::m_bMenu;
		}
		else if( Config::Binds->Menu == 4 ) // Mouse 3
		{
			if( message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK )
				Shared::m_bMenu = !Shared::m_bMenu;
		}
		else if( Config::Binds->Menu == 5 ) // Mouse 4
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON1 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bMenu = !Shared::m_bMenu;
			}
		}
		else if( Config::Binds->Menu == 6 ) // Mouse 5
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON2 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bMenu = !Shared::m_bMenu;
			}
		}
		else
		{
			if( wParam == Config::Binds->Menu )
			{
				if( message == WM_KEYDOWN || message == WM_SYSKEYDOWN )
					Shared::m_bMenu = !Shared::m_bMenu;
			}
		}

		if( Config::Binds->Panic == 1 ) // Mouse 1
		{
			if( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK )
				Shared::m_bPanic = !Shared::m_bPanic;
		}
		else if( Config::Binds->Panic == 2 ) // Mouse 2
		{
			if( message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK )
				Shared::m_bPanic = !Shared::m_bPanic;
		}
		else if( Config::Binds->Panic == 4 ) // Mouse 3
		{
			if( message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK )
				Shared::m_bPanic = !Shared::m_bPanic;
		}
		else if( Config::Binds->Panic == 5 ) // Mouse 4
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON1 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bPanic = !Shared::m_bPanic;
			}
		}
		else if( Config::Binds->Panic == 6 ) // Mouse 5
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON2 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bPanic = !Shared::m_bPanic;
			}
		}
		else
		{
			if( wParam == Config::Binds->Panic )
			{
				if( message == WM_KEYDOWN || message == WM_SYSKEYDOWN )
					Shared::m_bPanic = !Shared::m_bPanic;
			}
		}

		if( Config::Binds->Eject == 1 ) // Mouse 1
		{
			if( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK )
				Shared::m_bEject = true;
		}
		else if( Config::Binds->Eject == 2 ) // Mouse 2
		{
			if( message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK )
				Shared::m_bEject = true;
		}
		else if( Config::Binds->Eject == 4 ) // Mouse 3
		{
			if( message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK )
				Shared::m_bEject = true;
		}
		else if( Config::Binds->Eject == 5 ) // Mouse 4
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON1 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bEject = true;
			}
		}
		else if( Config::Binds->Eject == 6 ) // Mouse 5
		{
			WORD wKey = HIWORD( wParam );

			if( wKey == XBUTTON2 )
			{
				if( message == WM_XBUTTONDOWN || message == WM_XBUTTONDBLCLK )
					Shared::m_bEject = true;
			}
		}
		else
		{
			if( wParam == Config::Binds->Eject )
			{
				if( message == WM_KEYDOWN || message == WM_SYSKEYDOWN )
					Shared::m_bEject = true;
			}
		}
	}

	LRESULT WINAPI Win32::Proxy( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		Extra( message, wParam, lParam );

		if( Source::m_pMenu )
		{
			if( Source::m_pMenu->OnKeyEvent( hWnd, message, wParam, lParam ) )
				return FALSE;
		}

		if( Source::m_pAimbot )
			Source::m_pAimbot->OnKeyEvent( message, wParam, lParam );

		if( Source::m_pTriggerbot )
			Source::m_pTriggerbot->OnKeyEvent( message, wParam, lParam );

		Speed( message, wParam, lParam );
		AirStuck( message, wParam, lParam );

		return CallWindowProc( Source::m_pTargetInput->m_pProcedure, hWnd, message, wParam, lParam );
	}
}