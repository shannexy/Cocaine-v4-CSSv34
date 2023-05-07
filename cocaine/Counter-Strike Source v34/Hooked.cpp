#include "Hooked.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Config.hpp"

bool bSendPacket = false;

void ApplyStrafe(CUserCmd* cmd, const Vector3& va)
{
	float yaw, speed;

	Vector3& move = *(Vector3*)&cmd->forwardmove;

	speed = move.Length2D();

	yaw = ToDegrees(atan2(move.y, move.x));
	yaw = ToRadians(cmd->viewangles.y - va.y + yaw);

	move.x = cos(yaw) * speed;
}

void AutoStrafe(CUserCmd* cmd, C_CSPlayer* player)
{
	if (player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP)
		return;

	if (Source::m_pDataManager->GetFlags() & FL_ONGROUND)
		return;

	auto velocity = player->m_vecVelocity();

	float speed = velocity.Length2D();

	float tickrate = 1.0f;

	if ((Source::m_pGlobalVars->interval_per_tick * 100) > 1.0f)
		tickrate = 1.1f;

	static float yaw;

	yaw = AngleNormalize(cmd->viewangles.y - yaw);

	Vector3 strafe = cmd->viewangles;

	if (Config::Misc->AutoStrafe == 1) // Normal
	{
		if (yaw < 0.0f)
			cmd->sidemove = 400.0f;
		else if (yaw > 0.0f)
			cmd->sidemove = -400.0f;
	}
	else if (Config::Misc->AutoStrafe == 2) // Boost
	{
		float value = (8.15f - tickrate) - (speed / 340.0f);

		if (speed > 160.0f && speed < 420.0f)
			value = (4.6f - tickrate) - (speed / 340.0f);

		if (speed > 420.0f)
			value = (3.0f - tickrate) - (speed / 1000.0f);

		if (value <= 0.275f)
			value = 0.275f;

		if (abs(yaw) < value)
		{
			static bool direction = false;

			if (direction)
			{
				strafe.y -= value;
				cmd->sidemove = -400.0f;
			}
			else
			{
				strafe.y += value;
				cmd->sidemove = 400.0f;
			}

			direction = !direction;

			ApplyStrafe(cmd, strafe);
		}
		else
		{
			if (yaw < 0.0f)
				cmd->sidemove = 400.0f;
			else if (yaw > 0.0f)
				cmd->sidemove = -400.0f;
		}
	}

	yaw = cmd->viewangles.y;
}

void AutoJump(CUserCmd* cmd, C_CSPlayer* player)
{
	if (player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP)
		return;

	static bool bFirstJump = false;
	static bool bFakeJump = false;

	if (cmd->buttons & IN_JUMP)
	{
		if (!bFirstJump)
		{
			bFirstJump = bFakeJump = true;
		}
		else if (!(Source::m_pDataManager->GetFlags() & FL_ONGROUND))
		{
			if (bFakeJump && player->m_vecVelocity().z < 0.0f)
				bFakeJump = false;
			else
				cmd->buttons &= ~IN_JUMP;
		}
		else
		{
			bFakeJump = true;
		}
	}
	else
	{
		bFirstJump = false;
	}
}

void AutoPistol(CUserCmd* cmd, C_WeaponCSBaseGun* weapon)
{
	if (weapon->GetWeaponType() != WEAPONTYPE_PISTOL &&
		weapon->GetWeaponType() != WEAPONTYPE_SHOTGUN &&
		weapon->GetWeaponType() != WEAPONTYPE_SNIPER_RIFLE)
		return;

	if (!weapon->IsFireTime())
		cmd->buttons &= ~IN_ATTACK;
}

void NoSpread(CUserCmd* cmd, C_WeaponCSBaseGun* weapon)
{
	if (weapon->IsMelee())
		return;

	if (cmd->buttons & IN_ATTACK)
	{
		if (weapon->IsFireTime())
		{
			if (Config::Removals->SeedHelp)
			{
				cmd->random_seed = 141;

				while ((MD5_PseudoRandom(cmd->command_number) & 255) != cmd->random_seed)
					cmd->command_number++;
			}

			Source::m_pAccuracy->ApplySpreadFix(weapon, cmd->random_seed, cmd->viewangles, cmd->viewangles, Config::Removals->NoSpread);
		}
	}
}

void NoRecoil(CUserCmd* cmd, C_CSPlayer* player)
{
	Source::m_pAccuracy->ApplyRecoilFix(player, cmd->viewangles);
}

void AntiSMAC(CUserCmd* cmd)
{
	static Vector3 backup = cmd->viewangles;

	Vector3 angle = cmd->viewangles - backup;
	AnglesNormalize(angle);

	float change = angle.Length();

	if (change > 40.0f)
	{
		cmd->viewangles = backup + angle * 40.0f;
		AnglesNormalize(cmd->viewangles);

		cmd->buttons &= ~IN_ATTACK;
	}

	backup = cmd->viewangles;
}

void FakeLag(CUserCmd* cmd)
{
	static int fake_tick_count = 0;

	if (fake_tick_count == 0)
	{
		fake_tick_count = Config::Misc->ChokedPackets;

		bSendPacket = true;
	}
	else
	{
		fake_tick_count--;

		bSendPacket = false;
	}
}

void Speed()
{
	if (!Shared::m_bSpeed)
		return;

	static int speed_tick_count = 0;

	std::uintptr_t speed = 0;

	__asm
	{
		mov eax, fs: [0]
		mov speed, eax
	}

	if (speed_tick_count == 0)
	{
		speed_tick_count = Config::Misc->SpeedFactor;

		bSendPacket = true;
	}
	else
	{
		speed_tick_count--;

		bSendPacket = false;

		*(std::uintptr_t*)(speed + 12) -= 5;
	}
}

void AtTarget(CUserCmd* cmd, C_CSPlayer* player)
{
	auto best = -1;
	auto distance = 8192.0f;

	Vector3 end, aim;

	for (int i = 1; i <= Source::m_pEngine->GetMaxClients(); i++)
	{
		auto enemy = ToCSPlayer(Source::m_pEntList->GetBaseEntity(i));

		if (!enemy)
			continue;

		if (enemy == player)
			continue;

		if (enemy->m_lifeState() != LIFE_ALIVE)
			continue;

		if (enemy->IsDormant())
			continue;

		if (Config::Current->Aimbot->Target == 1) // Enemy
		{
			if (player->m_iTeamNum() == enemy->m_iTeamNum())
				continue;
		}
		else if (Config::Current->Aimbot->Target == 2) // Friendly
		{
			if (player->m_iTeamNum() != enemy->m_iTeamNum())
				continue;
		}

		if (!enemy->GetHitboxVector(12, end))
			continue;

		auto current = end.DistTo(player->EyePosition());

		if (current < distance)
		{
			distance = current;
			best = i;
		}
	}

	if (best != -1)
	{
		auto direction = end - player->EyePosition();

		VectorNormalize(direction);
		VectorAngles(direction, aim);

		cmd->viewangles.y = aim.y;
	}
}

void AntiAim(CUserCmd* cmd, C_CSPlayer* player, C_WeaponCSBaseGun* weapon)
{
	if (player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP)
		return;

	if ((cmd->buttons & IN_ATTACK) && weapon->IsFireTime())
		return;

	if (Config::AntiAim->AtTarget)
		AtTarget(cmd, player);

	if (Config::AntiAim->Pitch == 1) // Up
	{
		cmd->viewangles.x = -89.0f;
	}
	else if (Config::AntiAim->Pitch == 2) // Down
	{
		cmd->viewangles.x = 89.0f;
	}
	else if (Config::AntiAim->Pitch == 3) // Emotion Up
	{
		cmd->viewangles.x = -70.0f;
	}
	else if (Config::AntiAim->Pitch == 4) // Emotion Down
	{
		cmd->viewangles.x = 70.0f;
	}
	else if (Config::AntiAim->Pitch == 5) // Fake Up
	{
		cmd->viewangles.x = 180.0f;
	}

	if (Config::AntiAim->Yaw == 1) // Backwards
	{
		cmd->viewangles.y += 180.0f;
	}
	else if (Config::AntiAim->Yaw == 2) // Fake Forward
	{
		static int choked_tick_count = 0;

		if (choked_tick_count == 0)
		{
			choked_tick_count = Config::AntiAim->ChokedPackets;

			bSendPacket = true;
		}
		else
		{
			choked_tick_count--;

			bSendPacket = false;

			cmd->viewangles.y += 180.0f;
		}
	}
	else if (Config::AntiAim->Yaw == 3) // Sideways Left
	{
		cmd->viewangles.y += 90.0f;
	}
	else if (Config::AntiAim->Yaw == 4) // Sideways Right
	{
		cmd->viewangles.y -= 90.0f;
	}
	else if (Config::AntiAim->Yaw == 5) // Fake Sideways Left
	{
		static int choked_tick_count = 0;

		if (choked_tick_count == 0)
		{
			choked_tick_count = Config::AntiAim->ChokedPackets;

			bSendPacket = true;

			cmd->viewangles.y += 90.0f;
		}
		else
		{
			choked_tick_count--;

			bSendPacket = false;

			cmd->viewangles.y -= 90.0f;
		}
	}
	else if (Config::AntiAim->Yaw == 6) // Fake Sideways Right
	{
		static int choked_tick_count = 0;

		if (choked_tick_count == 0)
		{
			choked_tick_count = Config::AntiAim->ChokedPackets;

			bSendPacket = true;

			cmd->viewangles.y -= 90.0f;
		}
		else
		{
			choked_tick_count--;

			bSendPacket = false;

			cmd->viewangles.y += 90.0f;
		}
	}
	else if (Config::AntiAim->Yaw == 7) // Slow Spin
	{
		static float yaw = 0.0f;

		yaw += 10.0f;

		cmd->viewangles.y += yaw;

		if (yaw > 360.0f)
			yaw = 0.0f;
	}
	else if (Config::AntiAim->Yaw == 8) // Fast Spin
	{
		static float yaw = 0.0f;

		yaw += 45.0f;

		cmd->viewangles.y += yaw;

		if (yaw > 360.0f)
			yaw = 0.0f;
	}
	else if (Config::AntiAim->Yaw == 9) // Jitter
	{
		int type = cmd->command_number % 3;

		if (type == 1)
		{
			cmd->viewangles.y -= 160.0f;
			bSendPacket = true;
		}
		else if (type == 2)
		{
			cmd->viewangles.y -= 200.0f;
			bSendPacket = false;
		}
	}
}

void PerfectSilent(CUserCmd* cmd, C_WeaponCSBaseGun* weapon)
{
	if (cmd->buttons & IN_ATTACK && weapon->IsFireTime())
		bSendPacket = false;
}

void AirStuck(CUserCmd* cmd, C_WeaponCSBaseGun* weapon)
{
	if (!Shared::m_bStuck)
		return;

	if (cmd->buttons & IN_ATTACK && weapon->IsFireTime())
		return;

	cmd->tick_count = 0xFFFFF;
}

void RemoveInterpolation()
{
	// 0x390028 client.dll
	static std::uintptr_t interpolation_list = (std::uintptr_t)GetModuleHandle(XorStr("client.dll")) + 0x390028;

	*(std::uint16_t*)(interpolation_list + 12) = 0xFFFF;
	*(std::uint16_t*)(interpolation_list + 18) = 0;
}

void __fastcall CreateMove(void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active)
{
	__try
	{
		Source::m_pClientSwap->VCall< CreateMoveFn >(IBaseClientDLL_CreateMove)(ecx, sequence_number, input_sample_frametime, active);

		bSendPacket = true;

		auto cmd = Source::m_pInput->GetUserCmd(sequence_number);

		if (cmd)
		{
			Config::OnCreateMove();

			auto player = C_CSPlayer::GetLocalPlayer();

			if (player)
			{
				Source::m_pDataManager->PreCreateMove(cmd, player);

				Source::m_pPlayerList->OnCreateMove();

				if (player->m_lifeState() == LIFE_ALIVE)
				{
					if (Config::Misc->Speed)
						Speed();

					if (Config::Misc->AutoJump)
						AutoJump(cmd, player);

					if (Config::Misc->AutoStrafe)
						AutoStrafe(cmd, player);

					auto weapon = player->GetActiveWeapon();

					if (weapon)
					{
						auto va = cmd->viewangles;

						if (Config::Current->Aimbot->Mode)
							Source::m_pAimbot->OnCreateMove(cmd);

						if (Config::Current->Triggerbot->Mode)
							Source::m_pTriggerbot->OnCreateMove(cmd);

						if (Config::Misc->FakeLag)
							FakeLag(cmd);

						if (Config::Removals->NoSpread)
							NoSpread(cmd, weapon);

						if (Config::Removals->NoRecoil)
							NoRecoil(cmd, player);

						if (Config::Current->Aimbot->Silent == 2)
							PerfectSilent(cmd, weapon);

						if (cmd->buttons & IN_ATTACK && !weapon->IsFireTime())
							cmd->viewangles = va;

						if (Config::AntiAim->AtTarget || Config::AntiAim->Pitch || Config::AntiAim->Yaw)
							AntiAim(cmd, player, weapon);

						if (Config::Misc->AutoPistol)
							AutoPistol(cmd, weapon);

						if (Config::Misc->AirStuck)
							AirStuck(cmd, weapon);

						if (Config::Misc->Restriction == 1)
							AntiSMAC(cmd);

						Source::MovementFix(cmd, va, Config::AntiAim->Pitch == 5);
					}
				}

				Source::m_pDataManager->PostCreateMove(player);
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Source::m_pClientSwap->VCall< CreateMoveFn >(IBaseClientDLL_CreateMove)(ecx, sequence_number, input_sample_frametime, active);
	}
}

void __declspec(naked) __fastcall Hooked_CreateMove(void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push    ebp
		mov     ebp, esp

		mov     bSendPacket, bl

		movzx	eax, active
		push	eax
		mov		eax, input_sample_frametime
		push	eax
		mov		eax, sequence_number
		push	eax
		call    CreateMove

		mov     bl, bSendPacket

		mov     esp, ebp
		pop     ebp

		retn    0xC
	}
}

void __fastcall Hooked_FrameStageNotify(void* ecx, void* edx, ClientFrameStage_t curStage)
{
	__try
	{
		Vector3 saved;

		auto player = C_CSPlayer::GetLocalPlayer();

		if (curStage == FRAME_RENDER_START)
		{
			if (player)
			{
				saved = player->m_vecPunchAngle();

				if (Config::Removals->NoVisualRecoil)
					player->m_vecPunchAngle().Set();

				player->m_flFlashMaxAlpha() = (float)Config::Removals->FlashAmount * 2.55f;
			}
		}

		Source::m_pClientSwap->VCall< FrameStageNotifyFn >(IBaseClientDLL_FrameStageNotify)(ecx, curStage);

		if (curStage == FRAME_RENDER_START)
		{
			if (player)
				player->m_vecPunchAngle() = saved;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Source::m_pClientSwap->VCall< FrameStageNotifyFn >(IBaseClientDLL_FrameStageNotify)(ecx, curStage);
	}
}

void __fastcall Hooked_RunCommand(void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
{
	__try
	{
		Source::m_pPredictionSwap->VCall< RunCommandFn >(IPrediction_RunCommand)(ecx, player, ucmd, moveHelper);

		if (player == C_CSPlayer::GetLocalPlayer())
			Source::m_pDataManager->OnDataUpdate();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Source::m_pPredictionSwap->VCall< RunCommandFn >(IPrediction_RunCommand)(ecx, player, ucmd, moveHelper);
	}
}

void __fastcall Hooked_FinishMove(void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, CMoveData* move)
{
	__try
	{
		Source::m_pPredictionSwap->VCall< FinishMoveFn >(IPrediction_FinishMove)(ecx, player, ucmd, move);

		*(Vector3*)(player + 0xEC) = player->m_vecVelocity() + player->m_vecBaseVelocity();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Source::m_pPredictionSwap->VCall< FinishMoveFn >(IPrediction_FinishMove)(ecx, player, ucmd, move);
	}
}

void __fastcall Hooked_Update(void* ecx, void* edx, bool recieved_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command)
{
	__try
	{
		Source::m_pPredictionSwap->VCall< UpdateFn >(IPrediction_Update)(ecx, true, validframe, incoming_acknowledged, outgoing_command);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Source::m_pPredictionSwap->VCall< UpdateFn >(IPrediction_Update)(ecx, recieved_new_world_update, validframe, incoming_acknowledged, outgoing_command);
	}
}

CUserCmd* __fastcall Hooked_GetUserCmd(void* ecx, void* edx, int sequence_number)
{
	__try
	{
		return Source::m_pInput->GetUserCmd(sequence_number);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

void __fastcall Hooked_ResetMouse(void* ecx, void* edx)
{
	__try
	{
		if (!Shared::m_bPanic)
		{
			if (Shared::m_bMenu)
				return;
		}

		Source::m_pInputSwap->VCall< ResetMouseFn >(IInput_ResetMouse)(ecx);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Source::m_pInputSwap->VCall< ResetMouseFn >(IInput_ResetMouse)(ecx);
	}
}

int __fastcall Hooked_DrawModelEx(void* ecx, void* edx, ModelRenderInfo_t* info)
{
	__try
	{
		if (!Shared::m_bPanic)
		{
			if (Config::Render->ChamsMode)
				Source::m_pRender->OnDrawModel(ecx, info);

			int iRet = Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);

			Source::m_pRender->ForceMaterial(Direct3D9::Color::Empty, nullptr, false);

			return iRet;
		}

		return Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);
	}
}

HRESULT D3DAPI Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	Source::m_pRenderer->OnLostDevice();
	Source::m_pMenu->OnLostDevice();

	//Source::m_pDeviceSwap->Restore();
	auto hRet = Source::m_pDeviceSwap->VCall< ResetFn >(IDirect3DDevice9_Reset)(pDevice, pPresentationParameters);
	//Source::m_pDeviceSwap->Replace();

	Source::m_pRenderer->OnResetDevice();
	Source::m_pMenu->OnResetDevice();
	return hRet;
}

void PaintEntity(C_CSPlayer* player)
{
	using Direct3D9::Color;

	if (player->IsDormant())
		return;

	if (player->m_lifeState() != LIFE_ALIVE)
		return;

	if (Config::ESP->Target == 1) // Enemy
	{
		if (player->m_iTeamNum() == C_CSPlayer::GetLocalPlayer()->m_iTeamNum())
			return;
	}
	else if (Config::ESP->Target == 2) // Friendly
	{
		if (player->m_iTeamNum() != C_CSPlayer::GetLocalPlayer()->m_iTeamNum())
			return;
	}

	Vector3 head;

	if (!player->GetHitboxVector(12, head))
		return;

	Color color;

	if (player->m_iTeamNum() == 2) // T
	{
		color = Config::Colors->T_ESP_Normal;

		if (Config::ESP->Colored && Source::TraceLine(head, player))
			color = Config::Colors->T_ESP_Colored;
	}
	else if (player->m_iTeamNum() == 3) // CT
	{
		color = Config::Colors->CT_ESP_Normal;

		if (Config::ESP->Colored && Source::TraceLine(head, player))
			color = Config::Colors->CT_ESP_Colored;
	}

	const auto& transform = player->m_rgflCoordinateFrame();

	Vector3 position(transform[0][3], transform[1][3], transform[2][3]);

	Vector3 mins = player->GetMins();
	Vector3 maxs = player->GetMaxs() + Vector3(0.0f, 0.0f, 10.0f);

	Vector3 points[] =
	{
		{ mins.x, mins.y, mins.z },
		{ mins.x, maxs.y, mins.z },
		{ maxs.x, maxs.y, mins.z },
		{ maxs.x, mins.y, mins.z },
		{ maxs.x, maxs.y, maxs.z },
		{ mins.x, maxs.y, maxs.z },
		{ mins.x, mins.y, maxs.z },
		{ maxs.x, mins.y, maxs.z },
	};

	Vector3 transformed[8];

	for (int i = 0; i < 8; i++)
		VectorTransform(points[i], transform, transformed[i]);

	Vector3 flb, brt, blb, frt, frb, brb, blt, flt;

	if (!Source::WorldToScreen(transformed[3], flb) ||
		!Source::WorldToScreen(transformed[0], blb) ||
		!Source::WorldToScreen(transformed[2], frb) ||
		!Source::WorldToScreen(transformed[6], blt) ||
		!Source::WorldToScreen(transformed[5], brt) ||
		!Source::WorldToScreen(transformed[4], frt) ||
		!Source::WorldToScreen(transformed[1], brb) ||
		!Source::WorldToScreen(transformed[7], flt))
		return;

	Vector3 screen[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++)
	{
		if (left > screen[i].x)
			left = screen[i].x;
		if (top < screen[i].y)
			top = screen[i].y;
		if (right < screen[i].x)
			right = screen[i].x;
		if (bottom > screen[i].y)
			bottom = screen[i].y;
	}

	int x = (int)std::round(left);
	int y = (int)std::round(bottom);

	int w = (int)std::round(right - left);
	int h = (int)std::round(top - bottom);

	if (Config::ESP->Box == 1)
	{
		if (Config::ESP->Outlined)
			Source::m_pRenderer->DrawBorderBoxOut(x, y, w, h, 1, color, Color(0, 0, 0, color.A));
		else
			Source::m_pRenderer->DrawBorderBox(x, y, w, h, 1, color);
	}
	else if (Config::ESP->Box == 2)
	{
		int line_w = (int)std::round(w / 4);
		int line_h = (int)std::round(h / 4);

		if (Config::ESP->Outlined)
		{
			Source::m_pRenderer->DrawRect(x - 1, y - 1, line_w + 2, 3, Color(0, 0, 0, color.A)); // top left -> right
			Source::m_pRenderer->DrawRect(x - 1, y - 1, 3, line_h + 2, Color(0, 0, 0, color.A)); // top left -> bottom

			Source::m_pRenderer->DrawRect(x - 1, y + h - line_h, 3, line_h + 2, Color(0, 0, 0, color.A)); // bottom left -> top
			Source::m_pRenderer->DrawRect(x - 1, y + h - 1, line_w + 2, 3, Color(0, 0, 0, color.A)); // bottom left -> right

			Source::m_pRenderer->DrawRect(x + w - line_w, y - 1, line_w + 2, 3, Color(0, 0, 0, color.A)); // top right -> left
			Source::m_pRenderer->DrawRect(x + w - 1, y - 1, 3, line_h + 2, Color(0, 0, 0, color.A)); // top right -> bottom

			Source::m_pRenderer->DrawRect(x + w - line_w, y + h - 1, line_w + 2, 3, Color(0, 0, 0, color.A)); // bottom right -> left
			Source::m_pRenderer->DrawRect(x + w - 1, y + h - line_h, 3, line_h + 2, Color(0, 0, 0, color.A)); // bottom right -> top
		}

		Source::m_pRenderer->DrawRect(x, y, line_w, 1, color); // top left -> right
		Source::m_pRenderer->DrawRect(x, y, 1, line_h, color); // top left -> bottom

		Source::m_pRenderer->DrawRect(x, y + h - line_h + 1, 1, line_h, color); // bottom left -> top
		Source::m_pRenderer->DrawRect(x, y + h, line_w, 1, color); // bottom left -> right

		Source::m_pRenderer->DrawRect(x + w - line_w + 1, y, line_w, 1, color); // top right -> left
		Source::m_pRenderer->DrawRect(x + w, y, 1, line_h, color); // top right -> bottom

		Source::m_pRenderer->DrawRect(x + w - line_w + 1, y + h, line_w, 1, color); // bottom right -> left
		Source::m_pRenderer->DrawRect(x + w, y + h - line_h + 1, 1, line_h, color); // bottom right -> top
	}

	int pad_h = 0;

	if (Config::ESP->Name)
	{
		player_info_t data;

		if (Source::m_pEngine->GetPlayerInfo(player->GetIndex(), &data))
			Source::m_pRenderer->DrawText(Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color::White, data.name);
	}

	if (Config::ESP->Weapon)
	{
		auto weapon = player->GetActiveWeapon();

		if (weapon)
		{
			auto name = weapon->GetName();

			if (name)
				Source::m_pRenderer->DrawText(Source::m_hFont, x + w / 2, y + h + 2, FONT_ALIGN_CENTER_H, Color::White, "%s", (name + 7));
		}
	}

	if (Config::ESP->AimSpot)
	{
		Vector3 spot;

		if (player->GetHitboxVector(Config::Current->Aimbot->Spot, spot))
		{
			Vector3 screen;
			if (Source::WorldToScreen(spot, screen))
				Source::m_pRenderer->DrawRectOut(screen.x - 1, screen.y - 1, 3, 3, Color::White);
		}
	}

	if (Config::ESP->Health == 1) // Text
	{
		int pad_w = 0;

		if (Config::ESP->Armor == 2)
			pad_w += 5;

		Source::m_pRenderer->DrawText(Source::m_hFont, x + w + 4 + pad_w, y, FONT_ALIGN_LEFT, Color::White, "H: %i", player->m_iHealth());

		pad_h += 12;
	}
	else if (Config::ESP->Health == 2) // Bar
	{
		int health = player->m_iHealth();

		if (health > 100)
			health = 100;

		int size_h = (int)std::round(((h + 1) * health) / 100);
		int real_h = (h + 1) - size_h;

		Source::m_pRenderer->DrawRect(x - 6, y - 1, 4, h + 3, Color::Black);
		Source::m_pRenderer->DrawRect(x - 5, y + real_h, 2, size_h, Color(255 - (player->m_iHealth() * 2.55f), player->m_iHealth() * 2.55f, 0));
	}

	if (Config::ESP->Armor == 1) // Text
	{
		Source::m_pRenderer->DrawText(Source::m_hFont, x + w + 4, y + pad_h, FONT_ALIGN_LEFT, Color::White, "A: %i", player->m_ArmorValue());

		pad_h += 13;
	}
	else if (Config::ESP->Armor == 2) // Bar
	{
		int armor = player->m_ArmorValue();

		if (armor > 100)
			armor = 100;

		int size_h = (int)std::round(((h + 1) * armor) / 100);
		int real_h = (h + 1) - size_h;

		Source::m_pRenderer->DrawRect(x + w + 3, y - 1, 4, h + 3, Color::Black);
		Source::m_pRenderer->DrawRect(x + w + 4, y + real_h, 2, size_h, Color(180, 180, 180));
	}

	if (Config::ESP->Skeleton)
	{
		auto pModel = player->GetModel();

		if (pModel)
		{
			auto pStudioHdr = Source::m_pModelInfoClient->GetStudioModel(pModel);

			if (pStudioHdr)
			{
				for (auto i = 0; i < pStudioHdr->numbones; i++)
				{
					auto pBone = pStudioHdr->GetBone(i);

					if (!pBone)
						continue;

					if (!(pBone->flags & 0x100))
						continue;

					if (pBone->parent == -1)
						continue;

					Vector3 vBone, vParent;
					Vector3 vBoneS, vParentS;

					player->GetBoneVector(i, vBone);
					player->GetBoneVector(pBone->parent, vParent);

					if (Source::WorldToScreen(vBone, vBoneS) && Source::WorldToScreen(vParent, vParentS))
						Source::m_pRenderer->DrawLine(vBoneS.x, vBoneS.y, vParentS.x, vParentS.y, Color::White);
				}
			}
		}
	}

	if (Config::ESP->Defusing)
	{
		if (Config::ESP->Name)
			y -= 16;

		if (player->m_bIsDefusing())
			Source::m_pRenderer->DrawText(Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color::White, XorStr("Defusing!"));
	}
}

float ScaleDamage(float damage, int armor)
{
	float ratio = 0.5f;
	float bonus = 0.5f;

	if (armor > 0)
	{
		float new_damage = damage * ratio;
		float new_armor = (damage - new_damage) * bonus;

		if (new_armor > static_cast<float>(armor))
		{
			new_armor = static_cast<float>(armor) * (1.0f / bonus);
			new_damage = damage - new_armor;
		}

		damage = new_damage;
	}

	return damage;
}

void PaintGround(C_BaseEntity* ent)
{
	using Direct3D9::Color;

	if (ent->IsDormant())
		return;

	auto name = ent->GetClientClass()->m_pNetworkName;

	auto world = ent->m_vecOrigin();

	if (world.IsZero())
		return;

	if (std::strcmp(name, XorStr("CPlantedC4")) == 0)
	{
		if (Config::ESP->Bomb)
		{
			Vector3 screen;

			if (Source::WorldToScreen(world, screen))
				Source::m_pRenderer->DrawText(Source::m_hFont, screen.x, screen.y, FONT_ALIGN_LEFT, Color::Red, XorStr("[ C4 ]"));
		}

		if (Config::Misc->BombWarning)
		{
			auto player = C_CSPlayer::GetLocalPlayer();

			Vector3 bomb_origin = ent->m_vecOrigin();
			Vector3 spot_top(bomb_origin.x, bomb_origin.y, bomb_origin.z + 8.0f);
			Vector3 spot_bottom(spot_top.x, spot_top.y, spot_top.z - 40.0f);

			CTraceFilterSimple trace(ent);

			Ray_t ray;
			ray.Set(spot_top, spot_bottom);

			trace_t tr;
			Source::m_pEngineTrace->TraceRay(ray, 0x200400B, &trace, &tr);

			float bomb_radius = 500.0f;

			if (tr.fraction != 1.0f)
				bomb_origin = tr.endpos + tr.plane.normal * 0.6f;

			float damage = bomb_radius;
			float radius = bomb_radius * 3.5f;

			float adjusted_damage, falloff, damage_percentage;

			Vector3 src = bomb_origin;
			src.z += 1.0f;

			Vector3 end = player->EyePosition();

			damage_percentage = 1.0f;
			falloff = (radius != 0.0f) ? damage / radius : 1.0f;

			Vector3 target = end - src;

			adjusted_damage = (damage - target.Length() * falloff) * damage_percentage;
			adjusted_damage = ScaleDamage(adjusted_damage, player->m_ArmorValue());

			if (adjusted_damage > 0.0f)
			{
				int w, h;
				Source::m_pEngine->GetScreenSize(w, h);

				w /= 2;

				Color color(Color::Red);

				if (adjusted_damage < 80.0f)
					color = Color::Yellow;
				else if (adjusted_damage < 20.0f)
					color = Color::Green;

				Source::m_pRenderer->DrawText(Source::m_hFont, w, 5.0f, FONT_ALIGN_CENTER_H, color, XorStr("Explosion Warning! [%.2f]"), adjusted_damage);
			}
		}
	}
}

void ESP()
{
	auto player = C_CSPlayer::GetLocalPlayer();

	if (!player)
		return;

	int size = Source::m_pEntList->GetHighestEntityIndex();

	for (int i = 0; i <= size; i++)
	{
		auto ent = Source::m_pEntList->GetBaseEntity(i);

		if (!ent)
			continue;

		PaintGround(ent);

		auto enemy = ToCSPlayer(ent);

		if (!enemy)
			continue;

		if (enemy == player)
			continue;

		PaintEntity(enemy);
	}
}

void Crosshair()
{
	using Direct3D9::Color;

	auto player = C_CSPlayer::GetLocalPlayer();

	if (!player)
		return;

	int w, h;
	Source::m_pEngine->GetScreenSize(w, h);

	int x = w / 2;
	int y = h / 2;

	if (Config::Misc->ShowRecoil)
	{
		int dx = w / 90;
		int dy = h / 90;

		x -= (dx * player->m_vecPunchAngle().y);
		y += (dy * player->m_vecPunchAngle().x);
	}

	if (Config::Misc->Crosshair == 1) // Dot
	{
		if (Config::Misc->Outlined)
			Source::m_pRenderer->DrawRect(x - 2, y - 2, 4, 4, Color::Black);

		Source::m_pRenderer->DrawRect(x - 1, y - 1, 2, 2, Config::Colors->Crosshair);
	}
	else if (Config::Misc->Crosshair == 2) // Cross
	{
		if (Config::Misc->Outlined)
		{
			Source::m_pRenderer->DrawRect(x - 11, y - 1, 23, 3, Color::Black);
			Source::m_pRenderer->DrawRect(x - 1, y - 11, 3, 23, Color::Black);
		}

		Source::m_pRenderer->DrawRect(x - 10, y, 21, 1, Config::Colors->Crosshair);
		Source::m_pRenderer->DrawRect(x, y - 10, 1, 21, Config::Colors->Crosshair);
	}
	else if (Config::Misc->Crosshair == 3) // Swastika
	{
		if (Config::Misc->Outlined)
		{
			Source::m_pRenderer->DrawRect(x - 11, y - 1, 23, 3, Color::Black);
			Source::m_pRenderer->DrawRect(x - 1, y - 11, 3, 23, Color::Black);

			Source::m_pRenderer->DrawRect(x - 1, y - 11, 13, 3, Color::Black); // top -> left
			Source::m_pRenderer->DrawRect(x - 11, y - 11, 3, 11, Color::Black); // top -> bottom
			Source::m_pRenderer->DrawRect(x + 9, y, 3, 12, Color::Black); // right -> bottom
			Source::m_pRenderer->DrawRect(x - 11, y + 9, 11, 3, Color::Black); // bottom -> left
		}

		Source::m_pRenderer->DrawRect(x - 10, y, 21, 1, Config::Colors->Crosshair);
		Source::m_pRenderer->DrawRect(x, y - 10, 1, 21, Config::Colors->Crosshair);

		Source::m_pRenderer->DrawRect(x, y - 10, 11, 1, Config::Colors->Crosshair); // top -> left
		Source::m_pRenderer->DrawRect(x - 10, y - 10, 1, 10, Config::Colors->Crosshair); // top -> bottom
		Source::m_pRenderer->DrawRect(x + 10, y, 1, 11, Config::Colors->Crosshair); // right -> bottom
		Source::m_pRenderer->DrawRect(x - 10, y + 10, 10, 1, Config::Colors->Crosshair); // bottom -> left
	}
}

void Menu()
{
	Source::m_pMenu->OnPresentDevice();
}

void Watermark()
{
	int w, h;
	Source::m_pEngine->GetScreenSize(w, h);
	Source::m_pRenderer->DrawText(Source::m_hFont, (float)w - 5.0f, 5.0f, FONT_ALIGN_RIGHT, Direct3D9::Color::White, XorStr("Cocaine [ 4.0 ]"));
}

void PresentProxy()
{
	__try
	{
		Source::m_pRenderer->Begin();

		if (!Shared::m_bPanic)
		{
			ESP();
			Crosshair();
			Menu();
			Watermark();
		}

		Source::m_pRenderer->End();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}
}

void __declspec(naked) Hooked_Present()
{
	static std::uintptr_t uJumpBack = 0;

	uJumpBack = Source::m_pPresentSwap->GetReturnLocation();

	__asm
	{
		call PresentProxy

		and esp, 0xFFFFFFF8
		sub esp, 0x0C

		jmp uJumpBack
	}
}

void Hooked_CL_RunPrediction(PREDICTION_REASON reason)
{
	static auto s_nSignonState = *(std::uintptr_t*)(Memory::PatternScan(XorStr("engine.dll"), XorStr("83 3D ?? ?? ?? ?? ?? A3 ?? ?? ?? ?? 75 47")) + 2);
	static auto s_nDeltaTick = *(std::uintptr_t*)(Memory::PatternScan(XorStr("engine.dll"), XorStr("83 3D ?? ?? ?? ?? ?? 7C 3E 8B 0D")) + 2);
	static auto s_last_command_ack = *(std::uintptr_t*)(Memory::PatternScan(XorStr("engine.dll"), XorStr("A1 ?? ?? ?? ?? 56 50 A1")) + 1);
	static auto s_lastoutgoingcommand = *(std::uintptr_t*)(Memory::PatternScan(XorStr("engine.dll"), XorStr("A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B 11 53 56")) + 1);
	static auto s_chokedcommands = *(std::uintptr_t*)(Memory::PatternScan(XorStr("engine.dll"), XorStr("8B 35 ?? ?? ?? ?? 03 F0 A1 ?? ?? ?? ??")) + 2);

	int nSignonState = *(int*)s_nSignonState;
	int nDeltaTick = *(int*)s_nDeltaTick;
	int last_command_ack = *(int*)s_last_command_ack;
	int lastoutgoingcommand = *(int*)s_lastoutgoingcommand;
	int chokedcommands = *(int*)s_chokedcommands;

	if (!(nSignonState == 6))
		return;

	if (nDeltaTick < 0)
		return;

	bool valid = nDeltaTick > 0;

	Source::m_pPrediction->Update(nDeltaTick, valid, last_command_ack, lastoutgoingcommand + chokedcommands);
}

void DT_BasePlayer_m_nTickBase(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	*(int*)(pOut) = pData->m_Value.m_Int;

	if (pStruct == C_CSPlayer::GetLocalPlayer())
		Source::m_pDataManager->OnDataRecieved();
}

void DT_BasePlayer_m_vecPunchAngle(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	*(Vector3*)(pOut) = pData->m_Value.m_Vector;
}

void DT_ParticleSmokeGrenade_m_flSpawnTime(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float Value = pData->m_Value.m_Float;

	if (Config::Removals->NoSmoke)
		Value = 0.0f;

	*(float*)(pOut) = Value;
}

void DT_CSPlayer_m_angEyeAnglesX(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float angle = pData->m_Value.m_Float;

	auto local = C_CSPlayer::GetLocalPlayer();
	auto player = (C_CSPlayer*)pStruct;
	auto player_from_list = Source::m_pPlayerList->GetPlayer(player->GetIndex());

	if (player && player_from_list)
	{
		if (player_from_list->m_pitch == 1) // Zero
			angle = 0.0f;
		else if (player_from_list->m_pitch == 2) // Up
			angle = -89.0f;
		else if (player_from_list->m_pitch == 3) // Down
			angle = 89.0f;
	}

	*(float*)(pOut) = angle;
}

void DT_CSPlayer_m_angEyeAnglesY(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float angle = pData->m_Value.m_Float;

	auto local = C_CSPlayer::GetLocalPlayer();
	auto player = (C_CSPlayer*)pStruct;
	auto player_from_list = Source::m_pPlayerList->GetPlayer(player->GetIndex());

	if (local && player && player_from_list)
	{
		if (player_from_list->m_yaw == 1) // Zero
		{
			angle = 0.0f;
		}
		else if (player_from_list->m_yaw == 2) // Reversed
		{
			angle += 180.0f;
		}
		else if (player_from_list->m_yaw == 3) // Forward ( At Local )
		{
			Vector3 start;

			if (player->GetHitboxVector(12, start))
			{
				Vector3 direction = local->EyePosition() - start;
				VectorNormalize(direction);

				Vector3 aim;
				VectorAngles(direction, aim);

				angle = aim.y;
			}
		}
		else if (player_from_list->m_yaw == 4) // Backward ( At Local )
		{
			Vector3 start;

			if (player->GetHitboxVector(12, start))
			{
				Vector3 direction = local->EyePosition() - start;
				VectorNormalize(direction);

				Vector3 aim;
				VectorAngles(direction, aim);

				angle = aim.y + 180.0f;
			}
		}
		else if (player_from_list->m_yaw == 5) // Sideway Left
		{
			Vector3 start;

			if (player->GetHitboxVector(12, start))
			{
				Vector3 direction = local->EyePosition() - start;
				VectorNormalize(direction);

				Vector3 aim;
				VectorAngles(direction, aim);

				angle = aim.y + 90.0f;
			}
		}
		else if (player_from_list->m_yaw == 6) // Sideway Right
		{
			Vector3 start;

			if (player->GetHitboxVector(12, start))
			{
				Vector3 direction = local->EyePosition() - start;
				VectorNormalize(direction);

				Vector3 aim;
				VectorAngles(direction, aim);

				angle = aim.y - 90.0f;
			}
		}
	}

	*(float*)(pOut) = angle;
}