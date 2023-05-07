#include "Config.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Source.hpp"
#include "Menu.hpp"

#include <iostream>
#include <fstream>

namespace Config
{
	MainList*		Main = nullptr;
	CurrentList*	Current = nullptr;
	CurrentList*	Weapon[ WEAPON_MAX ];

	ESPList*		ESP = nullptr;
	RenderList*		Render = nullptr;
	AntiAimList*	AntiAim = nullptr;
	RemovalsList*	Removals = nullptr;
	MiscList*		Misc = nullptr;
	ColorsList*		Colors = nullptr;
	BindsList*		Binds = nullptr;

	std::string		m_config;
	std::string		m_current;

	void Startup( HMODULE hMod )
	{
		Main = new MainList();
		Main->Aimbot = new AimbotList();
		Main->Triggerbot = new TriggerbotList();

		Current = new CurrentList();
		Current->Aimbot = new AimbotList();
		Current->Triggerbot = new TriggerbotList();

		for( int i = 0; i < WEAPON_MAX; i++ )
		{
			Weapon[ i ] = new CurrentList();

			Weapon[ i ]->Aimbot = new AimbotList();
			Weapon[ i ]->Triggerbot = new TriggerbotList();
		}
	
		ESP = new ESPList();
		Render = new RenderList();
		AntiAim = new AntiAimList();
		Removals = new RemovalsList();
		Misc = new MiscList();
		Colors = new ColorsList();
		Binds = new BindsList();

		Shared::m_pVars = new Shared::Vars();

		GetModuleFileName(hMod, Shared::m_pVars->m_loader, MAX_PATH);
		
		m_config = Shared::m_pVars->m_loader;
		m_config = m_config.substr(0, m_config.find_last_of("\\"));
		m_config.append( XorStr( "\\v34\\" ) );

		Shared::m_strConfig = "default";
		Shared::m_bLoad = true;
	}

	void Release()
	{
		Memory::SafeDelete( Main->Aimbot );
		Memory::SafeDelete( Main->Triggerbot );
		Memory::SafeDelete( Main );

		Memory::SafeDelete( Current->Aimbot );
		Memory::SafeDelete( Current->Triggerbot );
		Memory::SafeDelete( Current );

		for( int i = 0; i < WEAPON_MAX; i++ )
		{
			Memory::SafeDelete( Weapon[ i ]->Aimbot );
			Memory::SafeDelete( Weapon[ i ]->Triggerbot );

			Memory::SafeDelete( Weapon[ i ] );
		}

		Memory::SafeDelete( ESP );
		Memory::SafeDelete( Render );
		Memory::SafeDelete( AntiAim );
		Memory::SafeDelete( Removals );
		Memory::SafeDelete( Misc );
		Memory::SafeDelete( Colors );
		Memory::SafeDelete( Binds );
	}

	bool LoadBool( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );

		if( strcmp( szData, XorStr( "on" ) ) == 0 || strcmp( szData, XorStr( "true" ) ) == 0 || atoi( szData ) == 1 )
			return true;

		return false;
	}

	int LoadInt( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );
		return atoi( szData );
	}

	float LoadFloat( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );
		return ( float )atof( szData );
	}

	Color LoadColor( const std::string& strSection, const std::string& strName )
	{
		Color result;
		result.R = LoadInt( strSection, std::string( strName ).append( ".r" ) );
		result.G = LoadInt( strSection, std::string( strName ).append( ".g" ) );
		result.B = LoadInt( strSection, std::string( strName ).append( ".b" ) );
		result.A = LoadInt( strSection, std::string( strName ).append( ".a" ) );
		return result;
	}

	void SaveBool( const std::string& strSection, const std::string& strName, bool bValue )
	{
		const char* szData = bValue ? XorStr( "true" ) : XorStr( "false" );
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), szData, m_current.c_str() );
	}

	void SaveInt( const std::string& strSection, const std::string& strName, int iValue )
	{
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), std::to_string( iValue ).c_str(), m_current.c_str() );
	}

	void SaveFloat( const std::string& strSection, const std::string& strName, float flValue )
	{
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), std::to_string( flValue ).c_str(), m_current.c_str() );
	}

	void SaveColor( const std::string& strSection, const std::string& strName, const Color& color )
	{
		SaveInt( strSection, std::string( strName ).append( ".r" ), color.R );
		SaveInt( strSection, std::string( strName ).append( ".g" ), color.G );
		SaveInt( strSection, std::string( strName ).append( ".b" ), color.B );
		SaveInt( strSection, std::string( strName ).append( ".a" ), color.A );
	}

	bool Exists( const std::string& name )
	{
		std::ifstream f( name );
		return f.good();
	}

	bool Create( const std::string& name )
	{
		auto dir = name.substr( 0, name.find_last_of( "\\" ) );

		if( CreateDirectory( dir.c_str(), nullptr ) || GetLastError() == ERROR_ALREADY_EXISTS )
		{
			std::ofstream f( name );

			if( f.good() )
			{
				f.close();

				return true;
			}
		}

		return false;
	}

	void Load( const std::string& name )
	{
		auto current = name;

		if( current.find( XorStr( ".cfg" ) ) == std::string::npos )
			current.append( XorStr( ".cfg" ) );

		m_current = m_config + current;

		if( !Exists( m_current ) )
			Save( name );

		std::string main( XorStr( "main" ) );

		Main->AimbotWeaponConfig		= LoadBool( main, XorStr( "aimbot.weapon.config" ) );
		Main->Aimbot->Mode				= LoadInt( main, XorStr( "aimbot.mode" ) );
		Main->Aimbot->Key				= LoadInt( main, XorStr( "aimbot.key" ) );
		Main->Aimbot->AutoFire			= LoadBool( main, XorStr( "aimbot.auto.fire" ) );
		Main->Aimbot->AutoStop			= LoadBool( main, XorStr( "aimbot.auto.stop" ) );
		Main->Aimbot->AutoCrouch		= LoadBool( main, XorStr( "aimbot.auto.crouch" ) );
		Main->Aimbot->AutoReload		= LoadBool( main, XorStr( "aimbot.auto.reload" ) );
		Main->Aimbot->NoSwitch			= LoadBool( main, XorStr( "aimbot.no.switch" ) );
		Main->Aimbot->Spot				= LoadInt( main, XorStr( "aimbot.spot" ) );
		Main->Aimbot->SpotRandomize		= LoadBool( main, XorStr( "aimbot.spot.randomize" ) );
		Main->Aimbot->TargetSelection	= LoadInt( main, XorStr( "aimbot.target.selection" ) );
		Main->Aimbot->FieldOfView		= LoadFloat( main, XorStr( "aimbot.fov" ) );
		Main->Aimbot->Smooth			= LoadInt( main, XorStr( "aimbot.smooth" ) );
		Main->Aimbot->StepX				= LoadFloat( main, XorStr( "aimbot.step.vertical" ) );
		Main->Aimbot->StepY				= LoadFloat( main, XorStr( "aimbot.step.horizontal" ) );
		Main->Aimbot->SmoothX			= LoadFloat( main, XorStr( "aimbot.smooth.vertical" ) );
		Main->Aimbot->SmoothY			= LoadFloat( main, XorStr( "aimbot.smooth.horizontal" ) );
		Main->Aimbot->Duration			= LoadInt( main, XorStr( "aimbot.duration" ) );
		Main->Aimbot->Delay				= LoadInt( main, XorStr( "aimbot.delay" ) );
		Main->Aimbot->SwitchDelay		= LoadInt( main, XorStr( "aimbot.switch.delay" ) );
		Main->Aimbot->RCS				= LoadBool( main, XorStr( "aimbot.rcs" ) );
		Main->Aimbot->RCSDelay			= LoadInt( main, XorStr( "aimbot.rcs.delay" ) );
		Main->Aimbot->RCSAmountX		= LoadInt( main, XorStr( "aimbot.rcs.amount.vertical" ) );
		Main->Aimbot->RCSAmountY		= LoadInt( main, XorStr( "aimbot.rcs.amount.horizontal" ) );
		Main->Aimbot->AutoWall			= LoadBool( main, XorStr( "aimbot.autowall" ) );
		Main->Aimbot->MinDamage			= LoadInt( main, XorStr( "aimbot.min.damage" ) );
		Main->Aimbot->HitScan			= LoadInt( main, XorStr( "aimbot.hitscan" ) );
		Main->Aimbot->HitScanScale		= LoadFloat( main, XorStr( "aimbot.hitscan.scale" ) );
		Main->Aimbot->Target			= LoadInt( main, XorStr( "aimbot.target" ) );
		Main->Aimbot->Silent			= LoadInt( main, XorStr( "aimbot.silent" ) );

		Main->Aimbot->Clamp();

		Main->TriggerbotWeaponConfig	= LoadBool( main, XorStr( "triggerbot.weapon.config" ) );
		Main->Triggerbot->Mode			= LoadInt( main, XorStr( "triggerbot.mode" ) );
		Main->Triggerbot->Key			= LoadInt( main, XorStr( "triggerbot.key" ) );
		Main->Triggerbot->Accuracy		= LoadInt( main, XorStr( "triggerbot.accuracy" ) );
		Main->Triggerbot->Delay			= LoadInt( main, XorStr( "triggerbot.delay" ) );
		Main->Triggerbot->Burst			= LoadInt( main, XorStr( "triggerbot.burst" ) );
		Main->Triggerbot->Head			= LoadBool( main, XorStr( "triggerbot.head" ) );
		Main->Triggerbot->Chest			= LoadBool( main, XorStr( "triggerbot.chest" ) );
		Main->Triggerbot->Stomach		= LoadBool( main, XorStr( "triggerbot.stomach" ) );
		Main->Triggerbot->Arms			= LoadBool( main, XorStr( "triggerbot.arms" ) );
		Main->Triggerbot->Legs			= LoadBool( main, XorStr( "triggerbot.legs" ) );
		Main->Triggerbot->AutoWall		= LoadBool( main, XorStr( "triggerbot.autowall" ) );
		Main->Triggerbot->MinDamage		= LoadInt( main, XorStr( "triggerbot.min.damage" ) );
		Main->Triggerbot->Target		= LoadInt( main, XorStr( "triggerbot.target" ) );

		Main->Triggerbot->Clamp();

		ESP->Box						= LoadInt( main, XorStr( "esp.box" ) );
		ESP->Outlined					= LoadBool( main, XorStr( "esp.outlined" ) );
		ESP->Name						= LoadBool( main, XorStr( "esp.name" ) );
		ESP->Weapon						= LoadBool( main, XorStr( "esp.weapon" ) );
		ESP->AimSpot					= LoadBool( main, XorStr( "esp.aimspot" ) );
		ESP->Health						= LoadInt( main, XorStr( "esp.health" ) );
		ESP->Armor						= LoadInt( main, XorStr( "esp.armor" ) );
		ESP->Skeleton					= LoadBool( main, XorStr( "esp.skeleton" ) );
		ESP->Colored					= LoadBool( main, XorStr( "esp.colored" ) );
		ESP->Defusing					= LoadBool( main, XorStr( "esp.defusing" ) );
		ESP->Bomb						= LoadBool( main, XorStr( "esp.bomb" ) );
		ESP->Target						= LoadInt( main, XorStr( "esp.target" ) );

		Render->ChamsMode				= LoadInt( main, XorStr( "render.chams.mode" ) );
		Render->ChamsColored			= LoadBool( main, XorStr( "render.chams.colored" ) );
		Render->ChamsTarget				= LoadInt( main, XorStr( "render.chams.target" ) );

		AntiAim->AtTarget				= LoadBool( main, XorStr( "antiaim.at.target" ) );
		AntiAim->Pitch					= LoadInt( main, XorStr( "antiaim.pitch" ) );
		AntiAim->Yaw					= LoadInt( main, XorStr( "antiaim.yaw" ) );
		AntiAim->ChokedPackets			= LoadInt( main, XorStr( "antiaim.choked.packets" ) );

		AntiAim->Clamp();

		Removals->NoSpread				= LoadInt( main, XorStr( "removals.no.spread" ) );
		Removals->SeedHelp				= LoadBool( main, XorStr( "removals.seed.help" ) );
		Removals->NoRecoil				= LoadBool( main, XorStr( "removals.no.recoil" ) );
		Removals->NoVisualRecoil		= LoadBool( main, XorStr( "removals.no.visual.recoil" ) );
		Removals->NoSmoke				= LoadBool( main, XorStr( "removals.no.smoke" ) );
		Removals->FlashAmount			= LoadInt( main, XorStr( "removals.flash.amount" ) );

		Removals->Clamp();

		Misc->AutoJump					= LoadBool( main, XorStr( "misc.auto.jump" ) );
		Misc->AutoPistol				= LoadBool( main, XorStr( "misc.auto.pistol" ) );
		Misc->AutoStrafe				= LoadInt( main, XorStr( "misc.auto.strafe" ) );
		Misc->BombWarning				= LoadBool( main, XorStr( "misc.bomb.warning" ) );
		Misc->Crosshair					= LoadInt( main, XorStr( "misc.crosshair" ) );
		Misc->Outlined					= LoadBool( main, XorStr( "misc.crosshair.outlined" ) );
		Misc->Crosshair					= LoadBool( main, XorStr( "misc.crosshair.show.recoil" ) );
		Misc->FakeLag					= LoadBool( main, XorStr( "misc.fakelag" ) );
		Misc->ChokedPackets				= LoadInt( main, XorStr( "misc.fakelag.choked.packets" ) );
		Misc->AirStuck					= LoadBool( main, XorStr( "misc.airstuck" ) );
		Misc->StuckKey					= LoadInt( main, XorStr( "misc.airstuck.key" ) );
		Misc->Speed						= LoadBool( main, XorStr( "misc.speedhack" ) );
		Misc->SpeedKey					= LoadInt( main, XorStr( "misc.speedhack.key" ) );
		Misc->SpeedFactor				= LoadInt( main, XorStr( "misc.speedhack.factor" ) );
		Misc->Restriction				= LoadInt( main, XorStr( "misc.restriction" ) );

		Misc->Clamp();

		std::string colors( XorStr( "colors" ) );

		Colors->T_ESP_Normal			= LoadColor( colors, XorStr( "t.esp.normal" ) );
		Colors->T_ESP_Colored			= LoadColor( colors, XorStr( "t.esp.colored" ) );
		Colors->T_Chams_Normal			= LoadColor( colors, XorStr( "t.chams.normal" ) );
		Colors->T_Chams_Colored			= LoadColor( colors, XorStr( "t.chams.colored" ) );
		Colors->CT_ESP_Normal			= LoadColor( colors, XorStr( "ct.esp.normal" ) );
		Colors->CT_ESP_Colored			= LoadColor( colors, XorStr( "ct.esp.colored" ) );
		Colors->CT_Chams_Normal			= LoadColor( colors, XorStr( "ct.chams.normal" ) );
		Colors->CT_Chams_Colored		= LoadColor( colors, XorStr( "ct.chams.colored" ) );
		Colors->Crosshair				= LoadColor( colors, XorStr( "crosshair" ) );
		Colors->Main					= LoadColor( colors, XorStr( "main" ) );

		Source::m_pMenu->SetColors();
		Source::m_pMenu->ApplyColors();

		std::string binds( XorStr( "binds" ) );

		Binds->Menu						= LoadInt( binds, XorStr( "menu" ) );
		Binds->Eject					= LoadInt( binds, XorStr( "eject" ) );
		Binds->Panic					= LoadInt( binds, XorStr( "panic" ) );

		for( int i = 0; i < ARRAYSIZE( WeaponList ); i++ )
		{
			auto weapon = WeaponList[ i ];
			auto index = GetWeaponID( weapon );

			Weapon[ index ]->Aimbot->Mode				= LoadInt( weapon, XorStr( "aimbot.mode" ) );
			Weapon[ index ]->Aimbot->Key				= LoadInt( weapon, XorStr( "aimbot.key" ) );
			Weapon[ index ]->Aimbot->AutoFire			= LoadBool( weapon, XorStr( "aimbot.auto.fire" ) );
			Weapon[ index ]->Aimbot->AutoStop			= LoadBool( weapon, XorStr( "aimbot.auto.stop" ) );
			Weapon[ index ]->Aimbot->AutoCrouch			= LoadBool( weapon, XorStr( "aimbot.auto.crouch" ) );
			Weapon[ index ]->Aimbot->AutoReload			= LoadBool( weapon, XorStr( "aimbot.auto.reload" ) );
			Weapon[ index ]->Aimbot->NoSwitch			= LoadBool( weapon, XorStr( "aimbot.no.switch" ) );
			Weapon[ index ]->Aimbot->Spot				= LoadInt( weapon, XorStr( "aimbot.spot" ) );
			Weapon[ index ]->Aimbot->SpotRandomize		= LoadBool( weapon, XorStr( "aimbot.spot.randomize" ) );
			Weapon[ index ]->Aimbot->TargetSelection	= LoadInt( weapon, XorStr( "aimbot.target.selection" ) );
			Weapon[ index ]->Aimbot->FieldOfView		= LoadFloat( weapon, XorStr( "aimbot.fov" ) );
			Weapon[ index ]->Aimbot->Smooth				= LoadInt( weapon, XorStr( "aimbot.smooth" ) );
			Weapon[ index ]->Aimbot->StepX				= LoadFloat( weapon, XorStr( "aimbot.step.vertical" ) );
			Weapon[ index ]->Aimbot->StepY				= LoadFloat( weapon, XorStr( "aimbot.step.horizontal" ) );
			Weapon[ index ]->Aimbot->SmoothX			= LoadFloat( weapon, XorStr( "aimbot.smooth.vertical" ) );
			Weapon[ index ]->Aimbot->SmoothY			= LoadFloat( weapon, XorStr( "aimbot.smooth.horizontal" ) );
			Weapon[ index ]->Aimbot->Duration			= LoadInt( weapon, XorStr( "aimbot.duration" ) );
			Weapon[ index ]->Aimbot->Delay				= LoadInt( weapon, XorStr( "aimbot.delay" ) );
			Weapon[ index ]->Aimbot->SwitchDelay		= LoadInt( weapon, XorStr( "aimbot.switch.delay" ) );
			Weapon[ index ]->Aimbot->RCS				= LoadBool( weapon, XorStr( "aimbot.rcs" ) );
			Weapon[ index ]->Aimbot->RCSDelay			= LoadInt( weapon, XorStr( "aimbot.rcs.delay" ) );
			Weapon[ index ]->Aimbot->RCSAmountX			= LoadInt( weapon, XorStr( "aimbot.rcs.amount.vertical" ) );
			Weapon[ index ]->Aimbot->RCSAmountY			= LoadInt( weapon, XorStr( "aimbot.rcs.amount.horizontal" ) );
			Weapon[ index ]->Aimbot->AutoWall			= LoadBool( weapon, XorStr( "aimbot.autowall" ) );
			Weapon[ index ]->Aimbot->MinDamage			= LoadInt( weapon, XorStr( "aimbot.min.damage" ) );
			Weapon[ index ]->Aimbot->HitScan			= LoadInt( weapon, XorStr( "aimbot.hitscan" ) );
			Weapon[ index ]->Aimbot->HitScanScale		= LoadFloat( weapon, XorStr( "aimbot.hitscan.scale" ) );
			Weapon[ index ]->Aimbot->Target				= LoadInt( weapon, XorStr( "aimbot.target" ) );
			Weapon[ index ]->Aimbot->Silent				= LoadInt( weapon, XorStr( "aimbot.silent" ) );

			Weapon[ index ]->Aimbot->Clamp();

			Weapon[ index ]->Triggerbot->Mode			= LoadInt( weapon, XorStr( "triggerbot.mode" ) );
			Weapon[ index ]->Triggerbot->Key			= LoadInt( weapon, XorStr( "triggerbot.key" ) );
			Weapon[ index ]->Triggerbot->Accuracy		= LoadInt( weapon, XorStr( "triggerbot.accuracy" ) );
			Weapon[ index ]->Triggerbot->Delay			= LoadInt( weapon, XorStr( "triggerbot.delay" ) );
			Weapon[ index ]->Triggerbot->Burst			= LoadInt( weapon, XorStr( "triggerbot.burst" ) );
			Weapon[ index ]->Triggerbot->Head			= LoadBool( weapon, XorStr( "triggerbot.head" ) );
			Weapon[ index ]->Triggerbot->Chest			= LoadBool( weapon, XorStr( "triggerbot.chest" ) );
			Weapon[ index ]->Triggerbot->Stomach		= LoadBool( weapon, XorStr( "triggerbot.stomach" ) );
			Weapon[ index ]->Triggerbot->Arms			= LoadBool( weapon, XorStr( "triggerbot.arms" ) );
			Weapon[ index ]->Triggerbot->Legs			= LoadBool( weapon, XorStr( "triggerbot.legs" ) );
			Weapon[ index ]->Triggerbot->AutoWall		= LoadBool( weapon, XorStr( "triggerbot.autowall" ) );
			Weapon[ index ]->Triggerbot->MinDamage		= LoadInt( weapon, XorStr( "triggerbot.min.damage" ) );
			Weapon[ index ]->Triggerbot->Target			= LoadInt( weapon, XorStr( "triggerbot.target" ) );

			Weapon[ index ]->Triggerbot->Clamp();
		}
	}

	void Save( const std::string& name )
	{
		auto current = name;

		if( current.find( XorStr( ".cfg" ) ) == std::string::npos )
			current.append( XorStr( ".cfg" ) );

		m_current = m_config + current;

		if( !Exists( m_current ) )
		{
			if( !Create( m_current ) )
				DPRINT( XorStr( "[Config::Save] Can't create config directory or file!" ) );
		}

		std::string main( XorStr( "main" ) );

		Main->Aimbot->Clamp();

		SaveBool( main, XorStr( "aimbot.weapon.config" ), Main->AimbotWeaponConfig );
		SaveInt( main, XorStr( "aimbot.mode" ), Main->Aimbot->Mode );
		SaveInt( main, XorStr( "aimbot.key" ), Main->Aimbot->Key );
		SaveBool( main, XorStr( "aimbot.auto.fire" ), Main->Aimbot->AutoFire );
		SaveBool( main, XorStr( "aimbot.auto.stop" ), Main->Aimbot->AutoStop );
		SaveBool( main, XorStr( "aimbot.auto.crouch" ), Main->Aimbot->AutoCrouch );
		SaveBool( main, XorStr( "aimbot.auto.reload" ), Main->Aimbot->AutoReload );
		SaveBool( main, XorStr( "aimbot.no.switch" ), Main->Aimbot->NoSwitch );
		SaveInt( main, XorStr( "aimbot.spot" ), Main->Aimbot->Spot );
		SaveBool( main, XorStr( "aimbot.spot.randomize" ), Main->Aimbot->SpotRandomize );
		SaveInt( main, XorStr( "aimbot.target.selection" ), Main->Aimbot->TargetSelection );
		SaveFloat( main, XorStr( "aimbot.fov" ), Main->Aimbot->FieldOfView );
		SaveInt( main, XorStr( "aimbot.smooth" ), Main->Aimbot->Smooth );
		SaveFloat( main, XorStr( "aimbot.step.vertical" ), Main->Aimbot->StepX );
		SaveFloat( main, XorStr( "aimbot.step.horizontal" ), Main->Aimbot->StepY );
		SaveFloat( main, XorStr( "aimbot.smooth.vertical" ), Main->Aimbot->SmoothX );
		SaveFloat( main, XorStr( "aimbot.smooth.horizontal" ), Main->Aimbot->SmoothY );
		SaveInt( main, XorStr( "aimbot.duration" ), Main->Aimbot->Duration );
		SaveInt( main, XorStr( "aimbot.delay" ), Main->Aimbot->Delay );
		SaveInt( main, XorStr( "aimbot.switch.delay" ), Main->Aimbot->SwitchDelay );
		SaveBool( main, XorStr( "aimbot.rcs" ), Main->Aimbot->RCS );
		SaveInt( main, XorStr( "aimbot.rcs.delay" ), Main->Aimbot->RCSDelay );
		SaveInt( main, XorStr( "aimbot.rcs.amount.vertical" ), Main->Aimbot->RCSAmountX );
		SaveInt( main, XorStr( "aimbot.rcs.amount.horizontal" ), Main->Aimbot->RCSAmountY );
		SaveBool( main, XorStr( "aimbot.autowall" ), Main->Aimbot->AutoWall );
		SaveInt( main, XorStr( "aimbot.min.damage" ), Main->Aimbot->MinDamage );
		SaveInt( main, XorStr( "aimbot.hitscan" ), Main->Aimbot->HitScan );
		SaveFloat( main, XorStr( "aimbot.hitscan.scale" ), Main->Aimbot->HitScanScale );
		SaveInt( main, XorStr( "aimbot.target" ), Main->Aimbot->Target );
		SaveInt( main, XorStr( "aimbot.silent" ), Main->Aimbot->Silent );

		Main->Triggerbot->Clamp();

		SaveBool( main, XorStr( "triggerbot.weapon.config" ), Main->TriggerbotWeaponConfig );
		SaveInt( main, XorStr( "triggerbot.mode" ), Main->Triggerbot->Mode );
		SaveInt( main, XorStr( "triggerbot.key" ), Main->Triggerbot->Key );
		SaveInt( main, XorStr( "triggerbot.accuracy" ), Main->Triggerbot->Accuracy );
		SaveInt( main, XorStr( "triggerbot.delay" ), Main->Triggerbot->Delay );
		SaveInt( main, XorStr( "triggerbot.burst" ), Main->Triggerbot->Burst );
		SaveBool( main, XorStr( "triggerbot.head" ), Main->Triggerbot->Head );
		SaveBool( main, XorStr( "triggerbot.chest" ), Main->Triggerbot->Chest );
		SaveBool( main, XorStr( "triggerbot.stomach" ), Main->Triggerbot->Stomach );
		SaveBool( main, XorStr( "triggerbot.arms" ), Main->Triggerbot->Arms );
		SaveBool( main, XorStr( "triggerbot.legs" ), Main->Triggerbot->Legs );
		SaveBool( main, XorStr( "triggerbot.autowall" ), Main->Triggerbot->AutoWall );
		SaveInt( main, XorStr( "triggerbot.min.damage" ), Main->Triggerbot->MinDamage );
		SaveInt( main, XorStr( "triggerbot.target" ), Main->Triggerbot->Target );

		SaveInt( main, XorStr( "esp.box" ), ESP->Box );
		SaveBool( main, XorStr( "esp.outlined" ), ESP->Outlined );
		SaveBool( main, XorStr( "esp.name" ), ESP->Name );
		SaveBool( main, XorStr( "esp.weapon" ), ESP->Weapon );
		SaveBool( main, XorStr( "esp.aimspot" ), ESP->AimSpot );
		SaveInt( main, XorStr( "esp.health" ), ESP->Health );
		SaveInt( main, XorStr( "esp.armor" ), ESP->Armor );
		SaveBool( main, XorStr( "esp.skeleton" ), ESP->Skeleton );
		SaveBool( main, XorStr( "esp.colored" ), ESP->Colored );
		SaveBool( main, XorStr( "esp.defusing" ), ESP->Defusing );
		SaveBool( main, XorStr( "esp.bomb" ), ESP->Bomb );
		SaveInt( main, XorStr( "esp.target" ), ESP->Target );

		SaveInt( main, XorStr( "render.chams.mode" ), Render->ChamsMode );
		SaveBool( main, XorStr( "render.chams.colored" ), Render->ChamsColored );
		SaveInt( main, XorStr( "render.chams.target" ), Render->ChamsTarget );

		AntiAim->Clamp();

		SaveBool( main, XorStr( "antiaim.at.target" ), AntiAim->AtTarget );
		SaveInt( main, XorStr( "antiaim.pitch" ), AntiAim->Pitch );
		SaveInt( main, XorStr( "antiaim.yaw" ), AntiAim->Yaw );
		SaveInt( main, XorStr( "antiaim.choked.packets" ), AntiAim->ChokedPackets );

		Removals->Clamp();

		SaveInt( main, XorStr( "removals.no.spread" ), Removals->NoSpread );
		SaveBool( main, XorStr( "removals.seed.help" ), Removals->SeedHelp );
		SaveBool( main, XorStr( "removals.no.recoil" ), Removals->NoRecoil );
		SaveBool( main, XorStr( "removals.no.visual.recoil" ), Removals->NoVisualRecoil );
		SaveBool( main, XorStr( "removals.no.smoke" ), Removals->NoSmoke );
		SaveInt( main, XorStr( "removals.flash.amount" ), Removals->FlashAmount );

		Misc->Clamp();

		SaveBool( main, XorStr( "misc.auto.jump" ), Misc->AutoJump );
		SaveBool( main, XorStr( "misc.auto.pistol" ), Misc->AutoPistol );
		SaveInt( main, XorStr( "misc.auto.strafe" ), Misc->AutoStrafe );
		SaveBool( main, XorStr( "misc.bomb.warning" ), Misc->BombWarning );
		SaveInt( main, XorStr( "misc.crosshair" ), Misc->Crosshair );
		SaveBool( main, XorStr( "misc.crosshair.outlined" ), Misc->Outlined );
		SaveBool( main, XorStr( "misc.crosshair.show.recoil" ), Misc->ShowRecoil );
		SaveBool( main, XorStr( "misc.fakelag" ), Misc->FakeLag );
		SaveInt( main, XorStr( "misc.fakelag.choked.packets" ), Misc->ChokedPackets );
		SaveBool( main, XorStr( "misc.airstuck" ), Misc->AirStuck );
		SaveInt( main, XorStr( "misc.airstuck.key" ), Misc->StuckKey );
		SaveBool( main, XorStr( "misc.speedhack" ), Misc->Speed );
		SaveInt( main, XorStr( "misc.speedhack.key" ), Misc->SpeedKey );
		SaveInt( main, XorStr( "misc.speedhack.factor" ), Misc->SpeedFactor );
		SaveInt( main, XorStr( "misc.restriction" ), Misc->Restriction );

		std::string colors( XorStr( "colors" ) );

		SaveColor( colors, XorStr( "t.esp.normal" ), Colors->T_ESP_Normal );
		SaveColor( colors, XorStr( "t.esp.colored" ), Colors->T_ESP_Colored );
		SaveColor( colors, XorStr( "t.chams.normal" ), Colors->T_Chams_Normal );
		SaveColor( colors, XorStr( "t.chams.colored" ), Colors->T_Chams_Colored );
		SaveColor( colors, XorStr( "ct.esp.normal" ), Colors->CT_ESP_Normal );
		SaveColor( colors, XorStr( "ct.esp.colored" ), Colors->CT_ESP_Colored );
		SaveColor( colors, XorStr( "ct.chams.normal" ), Colors->CT_Chams_Normal );
		SaveColor( colors, XorStr( "ct.chams.colored" ), Colors->CT_Chams_Colored );
		SaveColor( colors, XorStr( "crosshair" ), Colors->Crosshair );
		SaveColor( colors, XorStr( "main" ), Colors->Main );

		std::string binds( XorStr( "binds" ) );

		SaveInt( binds, XorStr( "menu" ), Binds->Menu );
		SaveInt( binds, XorStr( "eject" ), Binds->Eject );
		SaveInt( binds, XorStr( "panic" ), Binds->Panic );

		for( int i = 0; i < ARRAYSIZE( WeaponList ); i++ )
		{
			auto weapon = WeaponList[ i ];
			auto index = GetWeaponID( weapon );

			Main->Aimbot->Clamp();

			SaveInt( weapon, XorStr( "aimbot.mode" ), Weapon[ index ]->Aimbot->Mode );
			SaveInt( weapon, XorStr( "aimbot.key" ), Weapon[ index ]->Aimbot->Key );
			SaveBool( weapon, XorStr( "aimbot.auto.fire" ), Weapon[ index ]->Aimbot->AutoFire );
			SaveBool( weapon, XorStr( "aimbot.auto.stop" ), Weapon[ index ]->Aimbot->AutoStop );
			SaveBool( weapon, XorStr( "aimbot.auto.crouch" ), Weapon[ index ]->Aimbot->AutoCrouch );
			SaveBool( weapon, XorStr( "aimbot.auto.reload" ), Weapon[ index ]->Aimbot->AutoReload );
			SaveBool( weapon, XorStr( "aimbot.no.switch" ), Weapon[ index ]->Aimbot->NoSwitch );
			SaveInt( weapon, XorStr( "aimbot.spot" ), Weapon[ index ]->Aimbot->Spot );
			SaveBool( weapon, XorStr( "aimbot.spot.randomize" ), Weapon[ index ]->Aimbot->SpotRandomize );
			SaveInt( weapon, XorStr( "aimbot.target.selection" ), Weapon[ index ]->Aimbot->TargetSelection );
			SaveFloat( weapon, XorStr( "aimbot.fov" ), Weapon[ index ]->Aimbot->FieldOfView );
			SaveInt( weapon, XorStr( "aimbot.smooth" ), Weapon[ index ]->Aimbot->Smooth );
			SaveFloat( weapon, XorStr( "aimbot.step.vertical" ), Weapon[ index ]->Aimbot->StepX );
			SaveFloat( weapon, XorStr( "aimbot.step.horizontal" ), Weapon[ index ]->Aimbot->StepY );
			SaveFloat( weapon, XorStr( "aimbot.smooth.vertical" ), Weapon[ index ]->Aimbot->SmoothX );
			SaveFloat( weapon, XorStr( "aimbot.smooth.horizontal" ), Weapon[ index ]->Aimbot->SmoothY );
			SaveInt( weapon, XorStr( "aimbot.duration" ), Weapon[ index ]->Aimbot->Duration );
			SaveInt( weapon, XorStr( "aimbot.delay" ), Weapon[ index ]->Aimbot->Delay );
			SaveInt( weapon, XorStr( "aimbot.switch.delay" ), Weapon[ index ]->Aimbot->SwitchDelay );
			SaveBool( weapon, XorStr( "aimbot.rcs" ), Weapon[ index ]->Aimbot->RCS );
			SaveInt( weapon, XorStr( "aimbot.rcs.delay" ), Weapon[ index ]->Aimbot->RCSDelay );
			SaveInt( weapon, XorStr( "aimbot.rcs.amount.vertical" ), Weapon[ index ]->Aimbot->RCSAmountX );
			SaveInt( weapon, XorStr( "aimbot.rcs.amount.horizontal" ), Weapon[ index ]->Aimbot->RCSAmountY );
			SaveBool( weapon, XorStr( "aimbot.autowall" ), Weapon[ index ]->Aimbot->AutoWall );
			SaveInt( weapon, XorStr( "aimbot.min.damage" ), Weapon[ index ]->Aimbot->MinDamage );
			SaveInt( weapon, XorStr( "aimbot.hitscan" ), Weapon[ index ]->Aimbot->HitScan );
			SaveFloat( weapon, XorStr( "aimbot.hitscan.scale" ), Weapon[ index ]->Aimbot->HitScanScale );
			SaveInt( weapon, XorStr( "aimbot.target" ), Weapon[ index ]->Aimbot->Target );
			SaveInt( weapon, XorStr( "aimbot.silent" ), Weapon[ index ]->Aimbot->Silent );

			Weapon[ index ]->Triggerbot->Clamp();

			SaveInt( weapon, XorStr( "triggerbot.mode" ), Weapon[ index ]->Triggerbot->Mode );
			SaveInt( weapon, XorStr( "triggerbot.key" ), Weapon[ index ]->Triggerbot->Key );
			SaveInt( weapon, XorStr( "triggerbot.accuracy" ), Weapon[ index ]->Triggerbot->Accuracy );
			SaveInt( weapon, XorStr( "triggerbot.delay" ), Weapon[ index ]->Triggerbot->Delay );
			SaveInt( weapon, XorStr( "triggerbot.burst" ), Weapon[ index ]->Triggerbot->Burst );
			SaveBool( weapon, XorStr( "triggerbot.head" ), Weapon[ index ]->Triggerbot->Head );
			SaveBool( weapon, XorStr( "triggerbot.chest" ), Weapon[ index ]->Triggerbot->Chest );
			SaveBool( weapon, XorStr( "triggerbot.stomach" ), Weapon[ index ]->Triggerbot->Stomach );
			SaveBool( weapon, XorStr( "triggerbot.arms" ), Weapon[ index ]->Triggerbot->Arms );
			SaveBool( weapon, XorStr( "triggerbot.legs" ), Weapon[ index ]->Triggerbot->Legs );
			SaveBool( weapon, XorStr( "triggerbot.autowall" ), Weapon[ index ]->Triggerbot->AutoWall );
			SaveInt( weapon, XorStr( "triggerbot.min.damage" ), Weapon[ index ]->Triggerbot->MinDamage );
			SaveInt( weapon, XorStr( "triggerbot.target" ), Weapon[ index ]->Triggerbot->Target );
		}
	}

	void Delete( const std::string& name )
	{
		auto current = name;

		if( current.find( XorStr( ".cfg" ) ) == std::string::npos )
			current.append( XorStr( ".cfg" ) );

		m_current = m_config + current;

		DeleteFileA( m_current.c_str() );
	}

	void OnCreateMove()
	{
		auto player = C_CSPlayer::GetLocalPlayer();

		if( !player )
			return;

		auto weapon = player->GetActiveWeapon();

		if( !weapon )
			return;

		if( !Main || !Main->Aimbot || !Main->Triggerbot )
			return;

		auto i = weapon->GetWeaponID();

		if( Main->AimbotWeaponConfig )
			memcpy( Current->Aimbot, Weapon[ i ]->Aimbot, sizeof( AimbotList ) );
		else
			memcpy( Current->Aimbot, Main->Aimbot, sizeof( AimbotList ) );
		
		if( Main->TriggerbotWeaponConfig )
			memcpy( Current->Triggerbot, Weapon[ i ]->Triggerbot, sizeof( TriggerbotList ) );
		else
			memcpy( Current->Triggerbot, Main->Triggerbot, sizeof( TriggerbotList ) );
	}

	auto GetPath() -> std::string
	{
		return m_config;
	}

	const char* WeaponList[ 24 ]
	{
		"P228",
		"Glock-18",
		"Scout",
		"XM1014",
		"MAC-10",
		"AUG",
		"Dual Berettas",
		"Five-SeveN",
		"UMP-45",
		"SG 550",
		"Galil",
		"Famas",
		"USP",
		"AWP",
		"MP5",
		"M249",
		"M3",
		"M4A1",
		"TMP",
		"G3SG1",
		"Desert Eagle",
		"SG 552",
		"AK-47",
		"P90"
	};

	CSWeaponID GetWeaponID( const char* name )
	{
		if( !std::strcmp( name, XorStr( "P228" ) ) )
			return WEAPON_P228;
		else if( !std::strcmp( name, XorStr( "Glock-18" ) ) )
			return WEAPON_GLOCK;
		else if( !std::strcmp( name, XorStr( "Scout" ) ) )
			return WEAPON_SCOUT;
		else if( !std::strcmp( name, XorStr( "XM1014" ) ) )
			return WEAPON_XM1014;
		else if( !std::strcmp( name, XorStr( "MAC-10" ) ) )
			return WEAPON_MAC10;
		else if( !std::strcmp( name, XorStr( "AUG" ) ) )
			return WEAPON_AUG;
		else if( !std::strcmp( name, XorStr( "Dual Berettas" ) ) )
			return WEAPON_ELITE;
		else if( !std::strcmp( name, XorStr( "Five-SeveN" ) ) )
			return WEAPON_FIVESEVEN;
		else if( !std::strcmp( name, XorStr( "UMP-45" ) ) )
			return WEAPON_UMP45;
		else if( !std::strcmp( name, XorStr( "SG 550" ) ) )
			return WEAPON_SG550;
		else if( !std::strcmp( name, XorStr( "Galil" ) ) )
			return WEAPON_GALIL;
		else if( !std::strcmp( name, XorStr( "Famas" ) ) )
			return WEAPON_FAMAS;
		else if( !std::strcmp( name, XorStr( "USP" ) ) )
			return WEAPON_USP;
		else if( !std::strcmp( name, XorStr( "AWP" ) ) )
			return WEAPON_AWP;
		else if( !std::strcmp( name, XorStr( "MP5" ) ) )
			return WEAPON_MP5NAVY;
		else if( !std::strcmp( name, XorStr( "M249" ) ) )
			return WEAPON_M249;
		else if( !std::strcmp( name, XorStr( "M3" ) ) )
			return WEAPON_M3;
		else if( !std::strcmp( name, XorStr( "M4A1" ) ) )
			return WEAPON_M4A1;
		else if( !std::strcmp( name, XorStr( "TMP" ) ) )
			return WEAPON_TMP;
		else if( !std::strcmp( name, XorStr( "G3SG1" ) ) )
			return WEAPON_G3SG1;
		else if( !std::strcmp( name, XorStr( "Desert Eagle" ) ) )
			return WEAPON_DEAGLE;
		else if( !std::strcmp( name, XorStr( "SG 552" ) ) )
			return WEAPON_SG552;
		else if( !std::strcmp( name, XorStr( "AK-47" ) ) )
			return WEAPON_AK47;
		else if( !std::strcmp( name, XorStr( "P90" ) ) )
			return WEAPON_P90;
		else
			return WEAPON_NONE;
	}
}