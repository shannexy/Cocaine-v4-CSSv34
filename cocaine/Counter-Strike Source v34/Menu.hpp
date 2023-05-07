#pragma once

#include <d3d9.h>

#define COL_T_ESP_NORMAL		0
#define COL_T_ESP_COLORED		1
#define COL_T_CHAMS_NORMAL		2
#define COL_T_CHAMS_COLORED		3

#define COL_CT_ESP_NORMAL		4
#define COL_CT_ESP_COLORED		5
#define COL_CT_CHAMS_NORMAL		6
#define COL_CT_CHAMS_COLORED	7

#define COL_CROSSHAIR			8
#define COL_MAIN				9

namespace Feature
{
	class Menu
	{
	public:
		Menu();
		~Menu();

		bool	Create( HWND hWnd, IDirect3DDevice9* pDevice );

		void	OnPresentDevice();
		bool	OnKeyEvent( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		void	OnLostDevice();
		void	OnResetDevice();

	public:
		void	RenderPlayers();

		void	SetColors();
		void	ApplyColors();
		void	ResetColors();

	private:
		bool	m_bMouse;

		int		m_iWeaponAimbot;
		int		m_iWeaponTriggerbot;

		int		m_iConfig;
		bool	m_bConfigSelected[ 1024 ];
		char	m_szConfigName[ 256 ];

		bool	m_bAimbot;
		bool	m_bTriggerbot;
		bool	m_bESP;
		bool	m_bRender;
		bool	m_bAntiAim;
		bool	m_bRemovals;
		bool	m_bMisc;
		bool	m_bPlayers;
		bool	m_bColors;
		bool	m_bBinds;
		bool	m_bConfig;

		float	m_flColors[ 32 ][ 4 ];
	};
}