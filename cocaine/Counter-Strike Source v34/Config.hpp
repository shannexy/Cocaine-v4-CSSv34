#pragma once

#include "Valve.hpp"

#define LimitValue( val, min, max )	if( val < min ) val = min; if( val > max ) val = max;

namespace Config
{
	using Direct3D9::Color;

	struct AimbotList;
	struct TriggerbotList;

	struct MainList
	{
		bool	AimbotWeaponConfig		= false;
		bool	TriggerbotWeaponConfig	= false;

		AimbotList*		Aimbot		= nullptr;
		TriggerbotList*	Triggerbot	= nullptr;
	};

	struct CurrentList
	{
		AimbotList*		Aimbot		= nullptr;
		TriggerbotList*	Triggerbot	= nullptr;
	};

	struct AimbotList
	{
		int		Mode = 0;				// 0 - Off | 1 - Auto | 2 - On Press
		int		Key = 0;				// 
		bool	AutoFire = false;		// 
		bool	AutoStop = false;		// 
		bool	AutoCrouch = false;		// 
		bool	AutoReload = false;		// 
		bool	NoSwitch = false;		// 
		int		Spot = 0;				// 
		bool	SpotRandomize = false;	// 
		int		TargetSelection = 0;	// 
		float	FieldOfView = 0.0f;		// 
		int		Smooth = 0;				// 0 - Off | 1 - Step | 2 - Linear
		float	StepX = 0.0f;			// 0 - 100
		float	StepY = 0.0f;			// 0 - 100
		float	SmoothX = 0.0f;			// 0 - 100
		float	SmoothY = 0.0f;			// 0 - 100
		int		Duration = 0;			// ms ( 0 - 5000 )
		int		Delay = 0;				// ms ( 0 - 5000 )
		int		SwitchDelay = 0;		// ms ( 0 - 5000 ) Delay Before Aiming At Next Target
		bool	RCS = false;			// Recoil Control System
		int		RCSDelay = 0;			// bullets ( 0 - 10 )
		int		RCSAmountX = 0;			// percents ( 0 - 100 )
		int		RCSAmountY = 0;			// percents ( 0 - 100 )
		bool	AutoWall = false;		// Aim Through Penetrable Walls
		int		MinDamage = 0;			// Minimum Damage To Deal Through Wall
		int		HitScan = 0;			// 0 - Off | 1 - Normal | 2 - Corners
		float	HitScanScale = 1.0f;	// Corners Scale
		int		Target = 0;				// 0 - Everyone | 1 - Enemies | 2 - Friends
		int		Silent = 0;				// 0 - Off | 1 - Normal | 2 - Perfect

		void Clamp()
		{
			LimitValue( Key, 0, 128 );
			LimitValue( FieldOfView, 0.0f, 180.0f );
			LimitValue( StepX, 0.0f, 100.0f );
			LimitValue( StepY, 0.0f, 100.0f );
			LimitValue( SmoothX, 0.0f, 100.0f );
			LimitValue( SmoothY, 0.0f, 100.0f );
			LimitValue( Duration, 0, 5000 );
			LimitValue( Delay, 0, 5000 );
			LimitValue( SwitchDelay, 0, 5000 );
			LimitValue( RCSDelay, 0, 10 );
			LimitValue( RCSAmountX, 0, 100 );
			LimitValue( RCSAmountY, 0, 100 );
			LimitValue( MinDamage, 0, 100 );
			LimitValue( HitScanScale, 0.0f, 1.0f );
		}
	};

	struct TriggerbotList
	{
		int		Mode = 0;				// 0 - Off | 1 - Auto | 2 - On Press
		int		Key = 0;				// 
		int		Accuracy = 0;			// 0 - Normal | 1 - Perfect | 2 - Seed
		int		Delay = 0;				// 0 - 5000 ( ms )
		int		Burst = 0;				// 0 - 10 ( bullets )
		bool	Head = false;			// 
		bool	Chest = false;			// 
		bool	Stomach = false;		// 
		bool	Arms = false;			// 
		bool	Legs = false;			// 
		bool	AutoWall = false;		// 
		int		MinDamage = 0;			// 0 - 100
		int		Target = 0;				// 0 - Everyone | 1 - Enemy | 2 - Friendly

		void Clamp()
		{
			LimitValue( Key, 0, 128 );
			LimitValue( Delay, 0, 5000 );
			LimitValue( Burst, 0, 10 );
			LimitValue( MinDamage, 0, 100 );
		}
	};

	struct ESPList
	{
		int		Box = 0;			// 0 - Off | 1 - Normal | 2 - Corners
		bool	Outlined = false;	// Draw Black Outline
		
		bool	Name = false;		// Draw Player Name
		bool	Weapon = false;		// Draw Player Weapon
		bool	AimSpot = false;	// Draw Aim Spot

		int		Health = 0;			// 0 - Off | 1 - Text | 2 - Bar
		int		Armor = 0;			// 0 - Off | 1 - Text | 2 - Bar

		bool	Skeleton = false;	// Draw Player Skeleton
		bool	Colored = false;	// 
		bool	Defusing = false;	// Is Defusing Bomb
		bool	Bomb = false;		// Show Bomb

		int		Target = 0;			// 0 - Everyone | 1 - Enemy | 2 - Friendly
	};

	struct RenderList
	{
		int		ChamsMode = 0;			// 0 - Off | 1 - Flat | 2 - Shadow
		bool	ChamsColored = false;	// 
		int		ChamsTarget = 0;		// 0 - Everyone | 1 - Enemy | 2 - Friendly
	};

	struct AntiAimList
	{
		bool	AtTarget = false;		// 

		int		Pitch = 0;				// 0 - Off | 1 - Up | 2 - Down | 3 - Emotion Up | 4 - Emotion Down | 5 - Fake Up
		int		Yaw = 0;				// 0 - Off | 1 - Backward | 2 - Fake Forward | 3 - Sideways Left | 4 - Sideways Right | 5 - Fake Sideways Left | 6 - Fake Sideways Right | 7 - Slow Spin | 8 - Fast Spin | 9 - Jitter | 10 - Fake Spin

		int		ChokedPackets = 1;		// 1 - 15

		void Clamp()
		{
			LimitValue( ChokedPackets, 1, 15 );
		}
	};

	struct RemovalsList
	{
		int		NoSpread = 0;			// 0 - Off | 1 - Normal | 2 - Advanced
		bool	SeedHelp = false;		// 
		bool	NoRecoil = false;		// 
		bool	NoVisualRecoil = false;	// 
		
		bool	NoSmoke = false;		// 
		int		FlashAmount = 100;		// 0 - 100 ( percents )

		void Clamp()
		{
			LimitValue( FlashAmount, 0, 100 );
		}
	};

	struct MiscList
	{
		bool	AutoJump = false;		// 
		bool	AutoPistol = false;		// 
		int		AutoStrafe = 0;			// 0 - Off | 1 - Normal | 2 - Boost
		bool	BombWarning = false;	// 

		int		Crosshair = 0;			// 0 - Off | 1 - Dot | 2 - Cross | 3 - Swastika
		bool	Outlined = false;		// 
		bool	ShowRecoil = false;		// 

		bool	FakeLag = false;		// 
		int		ChokedPackets = 1;		// 1 - 15

		bool	AirStuck = false;		// 
		int		StuckKey = 0;			// 

		bool	Speed = false;			// 
		int		SpeedKey = 0;			// 
		int		SpeedFactor = 1;		// 1 - 20 

		int		Restriction = 0;		// 0 - Off | 1 - SMAC | 2 - Ultr@

		void Clamp()
		{
			LimitValue( ChokedPackets, 1, 15 );
			LimitValue( SpeedKey, 0, 128 );
			LimitValue( SpeedFactor, 1, 20 );
		}
	};

	struct ColorsList
	{
		Color T_ESP_Normal = Color( 255, 0, 0, 255 );		// 
		Color T_ESP_Colored = Color( 255, 255, 0, 255 );	// 

		Color T_Chams_Normal = Color( 255, 0, 0, 255 );		// 
		Color T_Chams_Colored = Color( 255, 255, 0, 255 );	// 
		
		Color CT_ESP_Normal = Color( 0, 128, 255, 255 );	// 
		Color CT_ESP_Colored = Color( 0, 255, 0, 255 );		// 

		Color CT_Chams_Normal = Color( 0, 128, 255, 255 );	// 
		Color CT_Chams_Colored = Color( 0, 255, 0, 255 );	// 

		Color Crosshair = Color( 255, 128, 0, 255 );		// crosshair color
		Color Main = Color( 255, 128, 0, 255 );				// main menu color
	};

	struct BindsList
	{
		int	Menu		= 45;		// VK_INSERT
		int Eject		= 122;		// VK_F11
		int Panic		= 123;		// VK_F12
	};

	extern MainList*		Main;
	extern CurrentList*		Current;
	extern CurrentList*		Weapon[ WEAPON_MAX ];

	extern ESPList*			ESP;
	extern RenderList*		Render;
	extern AntiAimList*		AntiAim;
	extern RemovalsList*	Removals;
	extern MiscList*		Misc;
	extern ColorsList*		Colors;
	extern BindsList*		Binds;

	extern void				Startup( HMODULE hMod );
	extern void				Release();

	extern void				Load( const std::string& name );
	extern void				Save( const std::string& name );
	extern void				Delete( const std::string& name );

	extern void				OnCreateMove();

	extern auto				GetPath() -> std::string;

	extern const char*		WeaponList[ 24 ];
	extern CSWeaponID		GetWeaponID( const char* name );
}