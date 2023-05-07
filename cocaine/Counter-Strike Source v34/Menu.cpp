#include "Menu.hpp"
#include "Source.hpp"
#include "Config.hpp"
#include "Player.hpp"

#include "ImGui.hpp"
#include "ImGuiDX9.hpp"

extern LRESULT ImGui_ImplDX9_WndProcHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

namespace Feature
{
	Menu::Menu()
		:	m_bMouse( false ),
			m_iWeaponAimbot( 0 ),
			m_iWeaponTriggerbot( 0 ),
			m_iConfig( -1 ),
			m_bAimbot( false ),
			m_bTriggerbot( false ),
			m_bESP( false ),
			m_bRender( false ),
			m_bAntiAim( false ),
			m_bRemovals( false ),
			m_bMisc( false ),
			m_bPlayers( false ),
			m_bColors( false ),
			m_bBinds( false ),
			m_bConfig( false )
	{
		ZeroMemory( m_szConfigName, sizeof( m_szConfigName ) );
	}

	Menu::~Menu()
	{
		Source::m_pCvar->FindVar( XorStr( "cl_mouseenable" ) )->m_nValue = 1;

		ImGui_ImplDX9_Shutdown();
	}

	bool Menu::Create( HWND hWnd, IDirect3DDevice9* pDevice )
	{
		if( !ImGui_ImplDX9_Init( hWnd, pDevice ) )
			return false;

		ImGuiStyle& Style = ImGui::GetStyle();

		Style.Colors[ ImGuiCol_Text ]                  = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
		Style.Colors[ ImGuiCol_TextDisabled ]          = ImVec4( 0.59f, 0.59f, 0.59f, 1.00f );
		Style.Colors[ ImGuiCol_WindowBg ]              = ImVec4( 0.16f, 0.16f, 0.16f, 1.00f );
		Style.Colors[ ImGuiCol_ChildWindowBg ]         = ImVec4( 0.10f, 0.10f, 0.10f, 0.10f );
		Style.Colors[ ImGuiCol_PopupBg ]               = ImVec4( 0.16f, 0.16f, 0.16f, 1.00f );
		Style.Colors[ ImGuiCol_Border ]                = ImVec4( 0.59f, 0.59f, 0.59f, 1.00f );
		Style.Colors[ ImGuiCol_BorderShadow ]          = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
		Style.Colors[ ImGuiCol_FrameBg ]               = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
		Style.Colors[ ImGuiCol_FrameBgHovered ]        = ImVec4( 0.24f, 0.24f, 0.24f, 0.61f );
		Style.Colors[ ImGuiCol_FrameBgActive ]         = ImVec4( 0.24f, 0.24f, 0.24f, 0.39f );
		Style.Colors[ ImGuiCol_TitleBg ]               = ImVec4( 0.02f, 0.02f, 0.02f, 1.00f );
		Style.Colors[ ImGuiCol_TitleBgCollapsed ]      = ImVec4( 0.02f, 0.02f, 0.02f, 1.00f );
		Style.Colors[ ImGuiCol_TitleBgActive ]         = ImVec4( 0.12f, 0.12f, 0.12f, 1.00f );
		Style.Colors[ ImGuiCol_MenuBarBg ]             = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
		Style.Colors[ ImGuiCol_ScrollbarBg ]           = ImVec4( 0.12f, 0.12f, 0.12f, 1.00f );
		Style.Colors[ ImGuiCol_ScrollbarGrab ]         = ImVec4( 1.00f, 0.50f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_ScrollbarGrabHovered ]  = ImVec4( 1.00f, 0.50f, 0.00f, 0.61f );
		Style.Colors[ ImGuiCol_ScrollbarGrabActive ]   = ImVec4( 1.00f, 0.50f, 0.00f, 0.39f );
		Style.Colors[ ImGuiCol_ComboBg ]               = ImVec4( 0.24f, 0.24f, 0.24f, 1.00f );
		Style.Colors[ ImGuiCol_CheckMark ]             = ImVec4( 1.00f, 0.50f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_SliderGrab ]            = ImVec4( 1.00f, 0.50f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_SliderGrabActive ]      = ImVec4( 1.00f, 0.50f, 0.00f, 0.61f );
		Style.Colors[ ImGuiCol_Button ]                = ImVec4( 1.00f, 0.50f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_ButtonHovered ]         = ImVec4( 1.00f, 0.50f, 0.00f, 0.61f );
		Style.Colors[ ImGuiCol_ButtonActive ]          = ImVec4( 1.00f, 0.50f, 0.00f, 0.39f );
		Style.Colors[ ImGuiCol_Header ]                = ImVec4( 1.00f, 0.50f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_HeaderHovered ]         = ImVec4( 1.00f, 0.50f, 0.00f, 0.61f );
		Style.Colors[ ImGuiCol_HeaderActive ]          = ImVec4( 1.00f, 0.50f, 0.00f, 0.39f );
		Style.Colors[ ImGuiCol_Column ]                = ImVec4( 0.59f, 0.59f, 0.59f, 1.00f );
		Style.Colors[ ImGuiCol_ColumnHovered ]         = ImVec4( 0.59f, 0.59f, 0.59f, 0.61f );
		Style.Colors[ ImGuiCol_ColumnActive ]          = ImVec4( 0.59f, 0.59f, 0.59f, 0.39f );
		Style.Colors[ ImGuiCol_ResizeGrip ]            = ImVec4( 1.00f, 1.00f, 1.00f, 0.30f );
		Style.Colors[ ImGuiCol_ResizeGripHovered ]     = ImVec4( 1.00f, 1.00f, 1.00f, 0.60f );
		Style.Colors[ ImGuiCol_ResizeGripActive ]      = ImVec4( 1.00f, 1.00f, 1.00f, 0.90f );
		Style.Colors[ ImGuiCol_CloseButton ]           = ImVec4( 1.00f, 0.50f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_CloseButtonHovered ]    = ImVec4( 1.00f, 0.50f, 0.00f, 0.61f );
		Style.Colors[ ImGuiCol_CloseButtonActive ]     = ImVec4( 1.00f, 0.50f, 0.00f, 0.39f );
		Style.Colors[ ImGuiCol_PlotLines ]             = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
		Style.Colors[ ImGuiCol_PlotLinesHovered ]      = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_PlotHistogram ]         = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_PlotHistogramHovered ]  = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
		Style.Colors[ ImGuiCol_TextSelectedBg ]        = ImVec4( 1.00f, 0.50f, 0.00f, 0.39f );
		Style.Colors[ ImGuiCol_ModalWindowDarkening ]  = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );

		Style.WindowPadding			= ImVec2( 8, 8 );
		Style.WindowRounding		= 2;
		Style.ChildWindowRounding	= 2;
		Style.FramePadding			= ImVec2( 4, 4 );
		Style.FrameRounding			= 2;
		Style.ItemSpacing			= ImVec2( 8, 4 );
		Style.ItemInnerSpacing		= ImVec2( 4, 4 );
		Style.TouchExtraPadding		= ImVec2( 0, 0 );
		Style.IndentSpacing			= 20;
		Style.ScrollbarSize			= 20;
		Style.ScrollbarRounding		= 2;
		Style.GrabRounding			= 2;

		SetColors();

		return true;
	}

	const char* ModeList[ ] =
	{
		"Off",
		"Auto",
		"On Press",
	};

	const char* SpotList[ ] =
	{
		"Pelvis",
		"Left Thigh",
		"Left Calf",
		"Left Foot",
		"Left Toe",
		"Right Thigh",
		"Right Calf",
		"Right Foot",
		"Right Toe",
		"Spine 1",
		"Spine 2",
		"Neck",
		"Head",
		"Left Upper Arm",
		"Left Forearm",
		"Left Hand",
		"Right Upper Arm",
		"Right Forearm",
		"Right Hand",
	};

	const char* TargetSelectionList[ ] =
	{
		"Fast",
		"Distance",
		"Crosshair",
		"Smart",
	};

	const char* SmoothList[ ] =
	{
		"Off",
		"Step",
		"Linear",
	};

	const char* HitScanList[ ] =
	{
		"Off",
		"Normal",
		"Extra",
	};

	const char* SilentList[ ] =
	{
		"Off",
		"Normal",
		"Perfect",
	};

	const char* AccuracyList[ ] =
	{
		"Normal",
		"Perfect",
		"Seed",
	};

	const char* TargetList[ ] =
	{
		"Everyone",
		"Enemy",
		"Friendly",
	};

	const char* BoxList[ ] =
	{
		"Off",
		"Normal",
		"Corners",
	};

	const char* InfoTypeList[ ] =
	{
		"Off",
		"Text",
		"Bar",
	};

	const char* InfoAlignList[ ] =
	{
		"Left",
		"Right",
		"Top",
		"Bottom",
	};

	const char* ChamsModeList[ ] =
	{
		"Off",
		"Flat",
		"Shadow",
	};

	const char* PitchList[ ] =
	{
		"Off",
		"Up",
		"Down",
		"Emotion Up",
		"Emotion Down",
		"Fake Up",
	};

	const char* YawList[ ] =
	{
		"Off",
		"Backward",
		"Fake Forward",
		"Sideway Left",
		"Sideway Right",
		"Fake Sideway Left",
		"Fake Sideway Right",
		"Slow Spin",
		"Fast Spin",
		"Jitter",
		"Fake Spin",
	};
	
	const char* NoSpreadList[ ] =
	{
		"Off",
		"Normal",
		"Advanced",
	};

	const char* AutoStrafeList[ ] =
	{
		"Off",
		"Normal",
		"Boost",
	};

	const char* CrosshairList[ ] =
	{
		"Off",
		"Dot",
		"Cross",
		"Swastika",
	};

	const char* RestrictionList[ ] =
	{
		"Off",
		"SMAC",
		// "Ultr@",
	};

	std::string GetExtension( const std::string& target )
	{
		auto found = target.find_last_of( "." );

		std::string extension( "" );

		if( found != std::string::npos )
			extension = target.substr( found + 1, target.length() );

		return extension;
	}

	void Menu::OnPresentDevice()
	{
		if( m_bMouse != Shared::m_bMenu )
		{
			m_bMouse = Shared::m_bMenu;

			Source::m_pCvar->FindVar( XorStr( "cl_mouseenable" ) )->m_nValue = ( int )!m_bMouse;
		}

		if( !Shared::m_bMenu )
			return;

		ImGui_ImplDX9_NewFrame();
		ImGui::GetIO().MouseDrawCursor = true;

		ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

		if( ImGui::BeginMainMenuBar() )
		{
			if( ImGui::BeginMenu( XorStr( "Cocaine v 4" ) ) )
			{
				ImGui::MenuItem( XorStr( "Aimbot" ), "", &m_bAimbot );
				ImGui::MenuItem( XorStr( "Triggerbot" ), "", &m_bTriggerbot );
				ImGui::MenuItem( XorStr( "ESP" ), "", &m_bESP );
				ImGui::MenuItem( XorStr( "Render" ), "", &m_bRender );
				ImGui::MenuItem( XorStr( "Anti Aim" ), "", &m_bAntiAim );
				ImGui::MenuItem( XorStr( "Removals" ), "", &m_bRemovals );
				ImGui::MenuItem( XorStr( "Misc" ), "", &m_bMisc );
				ImGui::MenuItem( XorStr( "Players" ), "", &m_bPlayers );
				ImGui::MenuItem( XorStr( "Colors" ), "", &m_bColors );
				ImGui::MenuItem( XorStr( "Binds" ), "", &m_bBinds );
				ImGui::MenuItem( XorStr( "Config" ), "", &m_bConfig );

				ImGui::Separator();

				if( ImGui::MenuItem( XorStr( "Close" ) ) )
				{
					m_bAimbot = false;
					m_bTriggerbot = false;
					m_bESP = false;
					m_bRender = false;
					m_bAntiAim = false;
					m_bRemovals = false;
					m_bMisc = false;
					m_bPlayers = false;
					m_bColors = false;
					m_bBinds = false;
					m_bConfig = false;
				}

				if( ImGui::MenuItem( XorStr( "Eject" ) ) )
				{
					Shared::m_bMenu = false;
					Shared::m_bEject = true;
				}
				
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if( m_bAimbot )
		{
			ImGui::Begin( XorStr( "Aimbot" ), nullptr, Flags );
			{
				ImGui::Checkbox( XorStr( "Weapon Config" ), &Config::Main->AimbotWeaponConfig );

				if( Config::Main->AimbotWeaponConfig )
				{
					CSWeaponID i = Config::GetWeaponID( Config::WeaponList[ m_iWeaponAimbot ] );
					ImGui::ListBox( "", &m_iWeaponAimbot, Config::WeaponList, ARRAYSIZE( Config::WeaponList ) );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Mode" ), &Config::Weapon[ i ]->Aimbot->Mode, ModeList, ARRAYSIZE( ModeList ) );
					if( Config::Weapon[ i ]->Aimbot->Mode == 2 ) // On Press
						ImGui::KeyButton( XorStr( "Key" ), &Config::Weapon[ i ]->Aimbot->Key );
					ImGui::Checkbox( XorStr( "Auto Fire" ), &Config::Weapon[ i ]->Aimbot->AutoFire );
					ImGui::Checkbox( XorStr( "Auto Stop" ), &Config::Weapon[ i ]->Aimbot->AutoStop );
					ImGui::Checkbox( XorStr( "Auto Crouch" ), &Config::Weapon[ i ]->Aimbot->AutoCrouch );
					ImGui::Checkbox( XorStr( "Auto Reload" ), &Config::Weapon[ i ]->Aimbot->AutoReload );
					ImGui::Checkbox( XorStr( "No Switch" ), &Config::Weapon[ i ]->Aimbot->NoSwitch );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Spot" ), &Config::Weapon[ i ]->Aimbot->Spot, SpotList, ARRAYSIZE( SpotList ) );
					ImGui::Checkbox( XorStr( "Randomize" ), &Config::Weapon[ i ]->Aimbot->SpotRandomize );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Target Selection" ), &Config::Weapon[ i ]->Aimbot->TargetSelection, TargetSelectionList, ARRAYSIZE( TargetSelectionList ) );
					if( Config::Weapon[ i ]->Aimbot->TargetSelection == 2 )
						ImGui::SliderFloat( XorStr( "Filed Of View" ), &Config::Weapon[ i ]->Aimbot->FieldOfView, 0.0f, 180.0f );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Smooth" ), &Config::Weapon[ i ]->Aimbot->Smooth, SmoothList, ARRAYSIZE( SmoothList ) );
					if( Config::Weapon[ i ]->Aimbot->Smooth == 1 ) // Step
					{
						ImGui::SliderFloat( XorStr( "Vertical" ), &Config::Weapon[ i ]->Aimbot->StepX, 0.0f, 100.0f );
						ImGui::SliderFloat( XorStr( "Horizontal" ), &Config::Weapon[ i ]->Aimbot->StepY, 0.0f, 100.0f );
					}
					else if( Config::Weapon[ i ]->Aimbot->Smooth == 2 ) // Linear
					{
						ImGui::SliderFloat( XorStr( "Vertical" ), &Config::Weapon[ i ]->Aimbot->SmoothX, 0.0f, 100.0f );
						ImGui::SliderFloat( XorStr( "Horizontal" ), &Config::Weapon[ i ]->Aimbot->SmoothY, 0.0f, 100.0f );
					}

					ImGui::Separator();

					ImGui::SliderInt( XorStr( "Duration" ), &Config::Weapon[ i ]->Aimbot->Duration, 0, 5000 );
					ImGui::SliderInt( XorStr( "Delay" ), &Config::Weapon[ i ]->Aimbot->Delay, 0, 5000 );
					ImGui::SliderInt( XorStr( "Switch Delay" ), &Config::Weapon[ i ]->Aimbot->SwitchDelay, 0, 5000 );

					ImGui::Separator();

					ImGui::Checkbox( XorStr( "RCS" ), &Config::Weapon[ i ]->Aimbot->RCS );
					if( Config::Weapon[ i ]->Aimbot->RCS )
					{
						ImGui::SliderInt( XorStr( "RCS Delay" ), &Config::Weapon[ i ]->Aimbot->RCSDelay, 0, 10 );
						ImGui::SliderInt( XorStr( "RCS Amount X" ), &Config::Weapon[ i ]->Aimbot->RCSAmountX, 0, 100, "%.0f%%" );
						ImGui::SliderInt( XorStr( "RCS Amount Y" ), &Config::Weapon[ i ]->Aimbot->RCSAmountY, 0, 100, "%.0f%%" );
					}

					ImGui::Separator();

					ImGui::Checkbox( XorStr( "Auto Wall" ), &Config::Weapon[ i ]->Aimbot->AutoWall );
					if( Config::Weapon[ i ]->Aimbot->AutoWall )
						ImGui::SliderInt( XorStr( "Min Damage" ), &Config::Weapon[ i ]->Aimbot->MinDamage, 0, 100 );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Hit Scan" ), &Config::Weapon[ i ]->Aimbot->HitScan, HitScanList, ARRAYSIZE( HitScanList ) );
					if( Config::Weapon[ i ]->Aimbot->HitScan == 2 ) // Extra
						ImGui::SliderFloat( XorStr( "Scale" ), &Config::Weapon[ i ]->Aimbot->HitScanScale, 0.0f, 1.0f );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Target" ), &Config::Weapon[ i ]->Aimbot->Target, TargetList, ARRAYSIZE( TargetList ) );
					ImGui::Combo( XorStr( "Silent" ), &Config::Weapon[ i ]->Aimbot->Silent, SilentList, ARRAYSIZE( SilentList ) );

					Config::Weapon[ i ]->Aimbot->Clamp();
				}
				else
				{
					ImGui::Separator();

					ImGui::Combo( XorStr( "Mode" ), &Config::Main->Aimbot->Mode, ModeList, ARRAYSIZE( ModeList ) );
					if( Config::Main->Aimbot->Mode == 2 ) // On Press
						ImGui::KeyButton( XorStr( "Key" ), &Config::Main->Aimbot->Key );
					ImGui::Checkbox( XorStr( "Auto Fire" ), &Config::Main->Aimbot->AutoFire );
					ImGui::Checkbox( XorStr( "Auto Stop" ), &Config::Main->Aimbot->AutoStop );
					ImGui::Checkbox( XorStr( "Auto Crouch" ), &Config::Main->Aimbot->AutoCrouch );
					ImGui::Checkbox( XorStr( "Auto Reload" ), &Config::Main->Aimbot->AutoReload );
					ImGui::Checkbox( XorStr( "No Switch" ), &Config::Main->Aimbot->NoSwitch );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Spot" ), &Config::Main->Aimbot->Spot, SpotList, ARRAYSIZE( SpotList ) );
					ImGui::Checkbox( XorStr( "Randomize" ), &Config::Main->Aimbot->SpotRandomize );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Target Selection" ), &Config::Main->Aimbot->TargetSelection, TargetSelectionList, ARRAYSIZE( TargetSelectionList ) );
					if( Config::Main->Aimbot->TargetSelection == 2 )
						ImGui::SliderFloat( XorStr( "Filed Of View" ), &Config::Main->Aimbot->FieldOfView, 0.0f, 180.0f );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Smooth" ), &Config::Main->Aimbot->Smooth, SmoothList, ARRAYSIZE( SmoothList ) );
					if( Config::Main->Aimbot->Smooth == 1 ) // Step
					{
						ImGui::SliderFloat( XorStr( "Vertical" ), &Config::Main->Aimbot->StepX, 0.0f, 100.0f );
						ImGui::SliderFloat( XorStr( "Horizontal" ), &Config::Main->Aimbot->StepY, 0.0f, 100.0f );
					}
					else if( Config::Main->Aimbot->Smooth == 2 ) // Linear
					{
						ImGui::SliderFloat( XorStr( "Vertical" ), &Config::Main->Aimbot->SmoothX, 0.0f, 100.0f );
						ImGui::SliderFloat( XorStr( "Horizontal" ), &Config::Main->Aimbot->SmoothY, 0.0f, 100.0f );
					}

					ImGui::Separator();

					ImGui::SliderInt( XorStr( "Duration" ), &Config::Main->Aimbot->Duration, 0, 5000 );
					ImGui::SliderInt( XorStr( "Delay" ), &Config::Main->Aimbot->Delay, 0, 5000 );
					ImGui::SliderInt( XorStr( "Switch Delay" ), &Config::Main->Aimbot->SwitchDelay, 0, 5000 );

					ImGui::Separator();

					ImGui::Checkbox( XorStr( "RCS" ), &Config::Main->Aimbot->RCS );
					if( Config::Main->Aimbot->RCS )
					{
						ImGui::SliderInt( XorStr( "RCS Delay" ), &Config::Main->Aimbot->RCSDelay, 0, 10 );
						ImGui::SliderInt( XorStr( "RCS Amount X" ), &Config::Main->Aimbot->RCSAmountX, 0, 100, "%.0f%%" );
						ImGui::SliderInt( XorStr( "RCS Amount Y" ), &Config::Main->Aimbot->RCSAmountY, 0, 100, "%.0f%%" );
					}

					ImGui::Separator();

					ImGui::Checkbox( XorStr( "Auto Wall" ), &Config::Main->Aimbot->AutoWall );
					if( Config::Main->Aimbot->AutoWall )
						ImGui::SliderInt( XorStr( "Min Damage" ), &Config::Main->Aimbot->MinDamage, 0, 100 );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Hit Scan" ), &Config::Main->Aimbot->HitScan, HitScanList, ARRAYSIZE( HitScanList ) );
					if( Config::Main->Aimbot->HitScan == 2 ) // Extra
						ImGui::SliderFloat( XorStr( "Scale" ), &Config::Main->Aimbot->HitScanScale, 0.0f, 1.0f );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Target" ), &Config::Main->Aimbot->Target, TargetList, ARRAYSIZE( TargetList ) );
					ImGui::Combo( XorStr( "Silent" ), &Config::Main->Aimbot->Silent, SilentList, ARRAYSIZE( SilentList ) );

					Config::Main->Aimbot->Clamp();
				}
			}
			ImGui::End();
		}

		if( m_bTriggerbot )
		{
			ImGui::Begin( XorStr( "Triggerbot" ), nullptr, Flags );
			{
				ImGui::Checkbox( XorStr( "Weapon Config" ), &Config::Main->TriggerbotWeaponConfig );

				if( Config::Main->AimbotWeaponConfig )
				{
					CSWeaponID i = Config::GetWeaponID( Config::WeaponList[ m_iWeaponTriggerbot ] );
					ImGui::ListBox( "", &m_iWeaponTriggerbot, Config::WeaponList, ARRAYSIZE( Config::WeaponList ) );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Mode" ), &Config::Weapon[ i ]->Triggerbot->Mode, ModeList, ARRAYSIZE( ModeList ) );
					if( Config::Weapon[ i ]->Triggerbot->Mode == 2 )
						ImGui::KeyButton( XorStr( "Key" ), &Config::Weapon[ i ]->Triggerbot->Key );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Accuracy" ), &Config::Weapon[ i ]->Triggerbot->Accuracy, AccuracyList, ARRAYSIZE( AccuracyList ) );
					ImGui::SliderInt( XorStr( "Delay" ), &Config::Weapon[ i ]->Triggerbot->Delay, 0, 5000 );
					ImGui::SliderInt( XorStr( "Burst" ), &Config::Weapon[ i ]->Triggerbot->Burst, 0, 10 );

					ImGui::Separator();

					ImGui::ListBoxHeader( "Spot" );

					ImGui::Selectable( "Head", &Config::Weapon[ i ]->Triggerbot->Head );
					ImGui::Selectable( "Chest", &Config::Weapon[ i ]->Triggerbot->Chest );
					ImGui::Selectable( "Stomach", &Config::Weapon[ i ]->Triggerbot->Stomach );
					ImGui::Selectable( "Arms", &Config::Weapon[ i ]->Triggerbot->Arms );
					ImGui::Selectable( "Legs", &Config::Weapon[ i ]->Triggerbot->Legs );

					ImGui::ListBoxFooter();

					ImGui::Separator();

					ImGui::Checkbox( "Auto Wall", &Config::Weapon[ i ]->Triggerbot->AutoWall );
					if( Config::Weapon[ i ]->Triggerbot->AutoWall )
						ImGui::SliderInt( "Min Damage", &Config::Weapon[ i ]->Triggerbot->MinDamage, 0, 100 );

					ImGui::Separator();

					ImGui::Combo( "Target", &Config::Weapon[ i ]->Triggerbot->Target, TargetList, ARRAYSIZE( TargetList ) );

					Config::Weapon[ i ]->Triggerbot->Clamp();
				}
				else
				{
					ImGui::Separator();
					
					ImGui::Combo( XorStr( "Mode" ), &Config::Main->Triggerbot->Mode, ModeList, ARRAYSIZE( ModeList ) );
					if( Config::Main->Triggerbot->Mode == 2 )
						ImGui::KeyButton( XorStr( "Key" ), &Config::Main->Triggerbot->Key );

					ImGui::Separator();

					ImGui::Combo( XorStr( "Accuracy" ), &Config::Main->Triggerbot->Accuracy, AccuracyList, ARRAYSIZE( AccuracyList ) );
					ImGui::SliderInt( XorStr( "Delay" ), &Config::Main->Triggerbot->Delay, 0, 5000 );
					ImGui::SliderInt( XorStr( "Burst" ), &Config::Main->Triggerbot->Burst, 0, 10 );
					
					ImGui::Separator();

					ImGui::ListBoxHeader( "Spot" );

					ImGui::Selectable( "Head", &Config::Main->Triggerbot->Head );
					ImGui::Selectable( "Chest", &Config::Main->Triggerbot->Chest );
					ImGui::Selectable( "Stomach", &Config::Main->Triggerbot->Stomach );
					ImGui::Selectable( "Arms", &Config::Main->Triggerbot->Arms );
					ImGui::Selectable( "Legs", &Config::Main->Triggerbot->Legs );

					ImGui::ListBoxFooter();

					ImGui::Separator();

					ImGui::Checkbox( "Auto Wall", &Config::Main->Triggerbot->AutoWall );
					if( Config::Main->Triggerbot->AutoWall )
						ImGui::SliderInt( "Min Damage", &Config::Main->Triggerbot->MinDamage, 0, 100 );

					ImGui::Separator();

					ImGui::Combo( "Target", &Config::Main->Triggerbot->Target, TargetList, ARRAYSIZE( TargetList ) );

					Config::Main->Triggerbot->Clamp();
				}
			}
			ImGui::End();
		}

		if( m_bESP )
		{
			ImGui::Begin( XorStr( "ESP" ), nullptr, Flags );
			{
				ImGui::Combo( XorStr( "Box" ), &Config::ESP->Box, BoxList, ARRAYSIZE( BoxList ) );
				ImGui::Checkbox( XorStr( "Outlined" ), &Config::ESP->Outlined );

				ImGui::Separator();
				
				ImGui::Checkbox( XorStr( "Name" ), &Config::ESP->Name );
				ImGui::Checkbox( XorStr( "Weapon" ), &Config::ESP->Weapon );
				ImGui::Checkbox( XorStr( "Aim Spot" ), &Config::ESP->AimSpot );

				ImGui::Separator();

				ImGui::Combo( XorStr( "Health" ), &Config::ESP->Health, InfoTypeList, ARRAYSIZE( InfoTypeList ) );
				ImGui::Combo( XorStr( "Armor" ), &Config::ESP->Armor, InfoTypeList, ARRAYSIZE( InfoTypeList ) );
				
				ImGui::Separator();

				ImGui::Checkbox( XorStr( "Skeleton" ), &Config::ESP->Skeleton );
				ImGui::Checkbox( XorStr( "Colored" ), &Config::ESP->Colored );
				ImGui::Checkbox( XorStr( "Defusing" ), &Config::ESP->Defusing );
				ImGui::Checkbox( XorStr( "Bomb" ), &Config::ESP->Bomb );

				ImGui::Separator();

				ImGui::Combo( XorStr( "Target" ), &Config::ESP->Target, TargetList, ARRAYSIZE( TargetList ) );
			}
			ImGui::End();
		}

		if( m_bRender )
		{
			ImGui::Begin( XorStr( "Render" ), nullptr, Flags );
			{
				ImGui::Text( XorStr( "Chams" ) );
				ImGui::Combo( XorStr( "Mode" ), &Config::Render->ChamsMode, ChamsModeList, ARRAYSIZE( ChamsModeList ) );
				if( Config::Render->ChamsMode != 0 )
				{
					ImGui::Checkbox( XorStr( "Colored" ), &Config::Render->ChamsColored );
					ImGui::Combo( XorStr( "Target" ), &Config::Render->ChamsTarget, TargetList, ARRAYSIZE( TargetList ) );
				}
			}
			ImGui::End();
		}

		if( m_bAntiAim )
		{
			ImGui::Begin( XorStr( "Anti Aim" ), nullptr, Flags );
			{
				ImGui::Checkbox( XorStr( "At Target" ), &Config::AntiAim->AtTarget );

				ImGui::Separator();
			
				ImGui::Combo( XorStr( "Pitch" ), &Config::AntiAim->Pitch, PitchList, ARRAYSIZE( PitchList ) );
				ImGui::Combo( XorStr( "Yaw" ), &Config::AntiAim->Yaw, YawList, ARRAYSIZE( YawList ) );

				if( Config::AntiAim->Yaw == 2 || Config::AntiAim->Yaw == 5 || Config::AntiAim->Yaw == 6 || Config::AntiAim->Yaw == 9 )
				{
					ImGui::Separator();
					ImGui::SliderInt( XorStr( "Choked Packets" ), &Config::AntiAim->ChokedPackets, 1, 15 );
				}

				Config::AntiAim->Clamp();
			}
			ImGui::End();
		}

		if( m_bRemovals )
		{
			ImGui::Begin( XorStr( "Removals" ), nullptr, Flags );
			{
				ImGui::Combo( XorStr( "No Spread" ), &Config::Removals->NoSpread, NoSpreadList, ARRAYSIZE( NoSpreadList ) );
				if( Config::Removals->NoSpread != 0 )
					ImGui::Checkbox( XorStr( "Seed Help" ), &Config::Removals->SeedHelp );

				ImGui::Separator();

				ImGui::Checkbox( XorStr( "No Recoil" ), &Config::Removals->NoRecoil );
				ImGui::Checkbox( XorStr( "No Visual Recoil" ), &Config::Removals->NoVisualRecoil );

				ImGui::Separator();

				ImGui::Checkbox( XorStr( "No Smoke" ), &Config::Removals->NoSmoke );
				ImGui::SliderInt( XorStr( "Flash Amount" ), &Config::Removals->FlashAmount, 0, 100, "%.0f%%" );

				Config::Removals->Clamp();
			}
			ImGui::End();
		}

		if( m_bMisc )
		{
			ImGui::Begin( XorStr( "Misc" ), nullptr, Flags );
			{
				ImGui::Checkbox( XorStr( "Auto Jump" ), &Config::Misc->AutoJump );
				ImGui::Checkbox( XorStr( "Auto Pistol" ), &Config::Misc->AutoPistol );
				ImGui::Combo( XorStr( "Auto Strafe" ), &Config::Misc->AutoStrafe, AutoStrafeList, ARRAYSIZE( AutoStrafeList ) );
				ImGui::Checkbox( XorStr( "Bomb Warning" ), &Config::Misc->BombWarning );

				ImGui::Separator();

				ImGui::Combo( XorStr( "Crosshair" ), &Config::Misc->Crosshair, CrosshairList, ARRAYSIZE( CrosshairList ) );
				if( Config::Misc->Crosshair )
				{
					ImGui::Checkbox( XorStr( "Outlined" ), &Config::Misc->Outlined );
					ImGui::Checkbox( XorStr( "Show Recoil" ), &Config::Misc->ShowRecoil );
				}

				ImGui::Separator();

				ImGui::Checkbox( XorStr( "Fake Lag" ), &Config::Misc->FakeLag );
				if( Config::Misc->FakeLag )
					ImGui::SliderInt( XorStr( "Choked Packets" ), &Config::Misc->ChokedPackets, 1, 15 );

				ImGui::Separator();

				ImGui::Checkbox( XorStr( "Air Stuck" ), &Config::Misc->AirStuck );
				if( Config::Misc->AirStuck )
					ImGui::KeyButton( XorStr( "Stuck Key" ), &Config::Misc->StuckKey );

				ImGui::Separator();

				ImGui::Checkbox( XorStr( "Speed" ), &Config::Misc->Speed );
				if( Config::Misc->Speed )
				{
					ImGui::KeyButton( XorStr( "Key" ), &Config::Misc->SpeedKey );
					ImGui::SliderInt( XorStr( "Factor" ), &Config::Misc->SpeedFactor, 1, 20 );
				}

				ImGui::Separator();

				ImGui::Combo( XorStr( "Restriction" ), &Config::Misc->Restriction, RestrictionList, ARRAYSIZE( RestrictionList ) );

				Config::Misc->Clamp();
			}
			ImGui::End();
		}

		if( m_bPlayers )
		{
			ImGui::SetNextWindowSize( { 500, 400 }, ImGuiSetCond_Appearing );
			ImGui::Begin( XorStr( "Players" ), nullptr, ImGuiWindowFlags_NoCollapse );
			{
				RenderPlayers();
			}
			ImGui::End();
		}

		if( m_bColors )
		{
			ImGui::Begin( XorStr( "Colors" ), nullptr, Flags );
			{
				ImGui::Text( XorStr( "ESP" ) );
				ImGui::BeginChild( XorStr( "ESP" ), ImVec2( 280, 110 ), true );
				{
					ImGui::ColorEdit4( XorStr( "T Normal" ), m_flColors[ COL_T_ESP_NORMAL ] );
					ImGui::ColorEdit4( XorStr( "T Colored" ), m_flColors[ COL_T_ESP_COLORED ] );
					ImGui::ColorEdit4( XorStr( "CT Normal" ), m_flColors[ COL_CT_ESP_NORMAL ] );
					ImGui::ColorEdit4( XorStr( "CT Colored" ), m_flColors[ COL_CT_ESP_COLORED ] );
				}
				ImGui::EndChild();

				ImGui::Text( XorStr( "Chams" ) );
				ImGui::BeginChild( XorStr( "Chams" ), ImVec2( 280, 110 ), true );
				{
					ImGui::ColorEdit4( XorStr( "T Normal" ), m_flColors[ COL_T_CHAMS_NORMAL ] );
					ImGui::ColorEdit4( XorStr( "T Colored" ), m_flColors[ COL_T_CHAMS_COLORED ] );
					ImGui::ColorEdit4( XorStr( "CT Normal" ), m_flColors[ COL_CT_CHAMS_NORMAL ] );
					ImGui::ColorEdit4( XorStr( "CT Colored" ), m_flColors[ COL_CT_CHAMS_COLORED ] );
				}
				ImGui::EndChild();

				ImGui::Text( XorStr( "Other" ) );
				ImGui::BeginChild( XorStr( "Other" ), ImVec2( 280, 90 ), true );
				{
					ImGui::ColorEdit3( XorStr( "Main" ), m_flColors[ COL_MAIN ] );
					ImGui::ColorEdit4( XorStr( "Crosshair" ), m_flColors[ COL_CROSSHAIR ] );
				}
				ImGui::EndChild();
				
				if( ImGui::Button( XorStr( "Apply" ) ) )
					ApplyColors();

				ImGui::SameLine();

				if( ImGui::Button( XorStr( "Reset" ) ) )
					ResetColors();
			}
			ImGui::End();
		}

		if( m_bBinds )
		{
			ImGui::Begin( XorStr( "Binds" ), nullptr, Flags );
			{
				ImGui::KeyButton( XorStr( "Menu" ), &Config::Binds->Menu );
				ImGui::KeyButton( XorStr( "Eject" ), &Config::Binds->Eject );
				ImGui::KeyButton( XorStr( "Panic" ), &Config::Binds->Panic );
			}
			ImGui::End();
		}

		if( m_bConfig )
		{
			ImGui::Begin( XorStr( "Config" ), nullptr, Flags );
			{
				ImGui::ListBoxHeader( "" );

				WIN32_FIND_DATAA ffd;
				LARGE_INTEGER filesize;
				CHAR szDir[ MAX_PATH ];
				HANDLE hFind = INVALID_HANDLE_VALUE;

				std::string strPath = Config::GetPath();

				strncpy_s( szDir, strPath.c_str(), MAX_PATH );
				strncat_s( szDir, "*", MAX_PATH );

				hFind = FindFirstFileA( szDir, &ffd );

				std::vector<std::string> files;

				int index = 0;

				if( hFind != INVALID_HANDLE_VALUE )
				{
					do
					{
						auto ext = GetExtension( ffd.cFileName );

						if( !( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && ext.compare( "cfg" ) == 0 )
						{
							const bool item_selected = ( index == m_iConfig );
							filesize.LowPart = ffd.nFileSizeLow;
							filesize.HighPart = ffd.nFileSizeHigh;
							char szText[ MAX_PATH ];
							sprintf_s( szText, sizeof( szText ), "%s [%lld bytes]", ffd.cFileName, filesize.QuadPart );
							ImGui::PushID( index );
							if( ImGui::Selectable( szText, item_selected ) )
							{
								m_iConfig = index;
								strncpy_s( m_szConfigName, ffd.cFileName, sizeof( m_szConfigName ) );
							}
							ImGui::PopID();
							index++;
							files.push_back( ffd.cFileName );
						}
					}
					while( FindNextFileA( hFind, &ffd ) != 0 );

					FindClose( hFind );
				}

				ImGui::ListBoxFooter();

				ImGui::InputText( "", m_szConfigName, sizeof( m_szConfigName ) );

				if( ImGui::Button( XorStr( "Load" ) ) && m_iConfig != -1 )
				{
					Shared::m_strConfig = files[ m_iConfig ];
					Shared::m_bLoad = true;
				}

				ImGui::SameLine();

				if( ImGui::Button( XorStr( "Save" ) ) && !std::string( m_szConfigName ).empty() )
				{
					Shared::m_strConfig = m_szConfigName;
					Shared::m_bSave = true;
				}

				ImGui::SameLine();

				if( ImGui::Button( XorStr( "Delete" ) ) )
					ImGui::OpenPopup( XorStr( "Delete?" ) );
				if( ImGui::BeginPopupModal( XorStr( "Delete?" ), NULL, ImGuiWindowFlags_AlwaysAutoResize ) )
				{
					ImGui::Text( "You are going to delete \"%s\"!\nAre you sure?\n\n", m_szConfigName );
					ImGui::Separator();

					if( ImGui::Button( XorStr( "Yes" ), ImVec2( 125, 0 ) ) )
					{
						Config::Delete( m_szConfigName );
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();

					if( ImGui::Button( XorStr( "No" ), ImVec2( 125, 0 ) ) )
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}
			}
			ImGui::End();
		}

		ImGui::Render();
	}

	bool Menu::OnKeyEvent( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		if( !Shared::m_bMenu )
			return false;

		if( ImGui_ImplDX9_WndProcHandler( hWnd, message, wParam, lParam ) )
			return true;

		return false;
	}

	void Menu::OnLostDevice()
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void Menu::OnResetDevice()
	{
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	const char* TeamList[ ] =
	{
		"None",
		"Spectator",
		"T",
		"CT",
	};

	const char* PitchModList[ ] =
	{
		"Normal",
		"Zero",
		"Up",
		"Down",
	};

	const char* YawModList[ ] =
	{
		"Normal",
		"Zero",
		"Reversed",
		"Forward",
		"Backward",
		"Sideway Left",
		"Sideway Right",
	};

	void Menu::RenderPlayers()
	{
		ImGui::Columns( 6, "players_columns" );
		ImGui::Separator();
		ImGui::Text( XorStr( "Name" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "Team" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "SteamID" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "UserID" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "Pitch" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "Yaw" ) ); ImGui::NextColumn();
		ImGui::Separator();

		auto local = C_CSPlayer::GetLocalPlayer();

		if( local )
		{
			int size = Source::m_pEngine->GetMaxClients();

			for( int i = 0; i <= size; i++ )
			{
				auto player = ToCSPlayer( Source::m_pEntList->GetBaseEntity( i ) );

				if( !player )
					continue;

				auto player_from_list = Source::m_pPlayerList->GetPlayer( i );

				if( !player_from_list )
					continue;

				int team = player->m_iTeamNum();

				if( team > 3 )
					team = 0;

				player_info_t data;
				if( !Source::m_pEngine->GetPlayerInfo( i, &data ) )
					continue;

				
				ImGui::Text( data.name ); ImGui::NextColumn();
				ImGui::Text( TeamList[ team ] ); ImGui::NextColumn();

				char guid[ 32 ];
				sprintf_s( guid, sizeof( guid ), "##guid_%i", i );

				ImGui::InputText( guid, data.guid, sizeof( data.guid ), ImGuiInputTextFlags_ReadOnly ); ImGui::NextColumn();
				ImGui::Text( std::to_string( data.userID ).c_str() ); ImGui::NextColumn();

				char pitch[ 32 ];
				sprintf_s( pitch, sizeof( pitch ), "##pitch_%i", i );

				char yaw[ 32 ];
				sprintf_s( yaw, sizeof( yaw ), "##yaw_%i", i );

				ImGui::Combo( pitch, &player_from_list->m_pitch, PitchModList, ARRAYSIZE( PitchModList ) ); ImGui::NextColumn();
				ImGui::Combo( yaw, &player_from_list->m_yaw, YawModList, ARRAYSIZE( YawModList ) ); ImGui::NextColumn();
			}
		}

		ImGui::Columns( 1 );
		ImGui::Separator();
	}

	void Menu::SetColors()
	{
		for( int i = 0; i < 4; i++ )
		{
			m_flColors[ COL_T_ESP_NORMAL ][ i ]		= Config::Colors->T_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_ESP_COLORED ][ i ]	= Config::Colors->T_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_NORMAL ][ i ]	= Config::Colors->T_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_COLORED ][ i ]	= Config::Colors->T_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CT_ESP_NORMAL ][ i ]	= Config::Colors->CT_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_ESP_COLORED ][ i ]	= Config::Colors->CT_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_NORMAL ][ i ]	= Config::Colors->CT_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_COLORED ][ i ]	= Config::Colors->CT_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CROSSHAIR ][ i ]		= Config::Colors->Crosshair[ i ] / 255.0f;
			m_flColors[ COL_MAIN ][ i ]				= Config::Colors->Main[ i ] / 255.0f;
		}
	}

	void Menu::ApplyColors()
	{
		for( int i = 0; i < 4; i++ )
		{
			Config::Colors->T_ESP_Normal[ i ]		= ( int )( m_flColors[ COL_T_ESP_NORMAL ][ i ] * 255.0f );
			Config::Colors->T_ESP_Colored[ i ]		= ( int )( m_flColors[ COL_T_ESP_COLORED ][ i ] * 255.0f );
			Config::Colors->T_Chams_Normal[ i ]		= ( int )( m_flColors[ COL_T_CHAMS_NORMAL ][ i ] * 255.0f );
			Config::Colors->T_Chams_Colored[ i ]	= ( int )( m_flColors[ COL_T_CHAMS_COLORED ][ i ] * 255.0f );

			Config::Colors->CT_ESP_Normal[ i ]		= ( int )( m_flColors[ COL_CT_ESP_NORMAL ][ i ] * 255.0f );
			Config::Colors->CT_ESP_Colored[ i ]		= ( int )( m_flColors[ COL_CT_ESP_COLORED ][ i ] * 255.0f );
			Config::Colors->CT_Chams_Normal[ i ]	= ( int )( m_flColors[ COL_CT_CHAMS_NORMAL ][ i ] * 255.0f );
			Config::Colors->CT_Chams_Colored[ i ]	= ( int )( m_flColors[ COL_CT_CHAMS_COLORED ][ i ] * 255.0f );

			Config::Colors->Crosshair[ i ]			= ( int )( m_flColors[ COL_CROSSHAIR ][ i ] * 255.0f );
			Config::Colors->Main[ i ]				= ( int )( m_flColors[ COL_MAIN ][ i ] * 255.0f );
		}

		for( int i = 0; i < 3; i++ )
		{
			ImGuiStyle& Style = ImGui::GetStyle();

			Style.Colors[ ImGuiCol_ScrollbarGrab ][ i ]			= m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_ScrollbarGrabHovered ][ i ]	= m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_ScrollbarGrabActive ][ i ]	= m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_CheckMark ][ i ]             = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_SliderGrab ][ i ]            = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_SliderGrabActive ][ i ]      = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_Button ][ i ]                = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_ButtonHovered ][ i ]         = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_ButtonActive ][ i ]          = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_Header ][ i ]                = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_HeaderHovered ][ i ]         = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_HeaderActive ][ i ]          = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_CloseButton ][ i ]           = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_CloseButtonHovered ][ i ]    = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_CloseButtonActive ][ i ]     = m_flColors[ COL_MAIN ][ i ];
			Style.Colors[ ImGuiCol_TextSelectedBg ][ i ]        = m_flColors[ COL_MAIN ][ i ];
		}
	}

	void Menu::ResetColors()
	{
		for( int i = 0; i < 4; i++ )
		{
			m_flColors[ COL_T_ESP_NORMAL ][ i ]		= Config::Colors->T_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_ESP_COLORED ][ i ]	= Config::Colors->T_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_NORMAL ][ i ]	= Config::Colors->T_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_COLORED ][ i ]	= Config::Colors->T_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CT_ESP_NORMAL ][ i ]	= Config::Colors->CT_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_ESP_COLORED ][ i ]	= Config::Colors->CT_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_NORMAL ][ i ]	= Config::Colors->CT_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_COLORED ][ i ]	= Config::Colors->CT_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CROSSHAIR ][ i ]		= Config::Colors->Crosshair[ i ] / 255.0f;
			m_flColors[ COL_MAIN ][ i ]				= Config::Colors->Main[ i ] / 255.0f;
		}

		for( int i = 0; i < 3; i++ )
		{
			ImGuiStyle& Style = ImGui::GetStyle();

			Style.Colors[ ImGuiCol_ScrollbarGrab ][ i ]			= Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_ScrollbarGrabHovered ][ i ]	= Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_ScrollbarGrabActive ][ i ]	= Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_CheckMark ][ i ]             = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_SliderGrab ][ i ]            = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_SliderGrabActive ][ i ]      = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_Button ][ i ]                = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_ButtonHovered ][ i ]         = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_ButtonActive ][ i ]          = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_Header ][ i ]                = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_HeaderHovered ][ i ]         = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_HeaderActive ][ i ]          = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_CloseButton ][ i ]           = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_CloseButtonHovered ][ i ]    = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_CloseButtonActive ][ i ]     = Config::Colors->Main[ i ] / 255.0f;
			Style.Colors[ ImGuiCol_TextSelectedBg ][ i ]        = Config::Colors->Main[ i ] / 255.0f;
		}
	}
}