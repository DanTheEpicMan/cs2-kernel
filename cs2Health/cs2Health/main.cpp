#include "memory.h"
#include <Windows.h>
#include <iostream>
#include <thread>

#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

float width = 2560;
float height = 1440;


struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

struct Vector3
{
	float x, y, z;
};

struct Vector2
{
	float x, y;
};

struct C_UTL_VECTOR
{
	DWORD_PTR count = 0;
	DWORD_PTR data = 0;
};

struct Vector4
{
	float x, y, z, w;
};

float offsetx(float x)
{
	x = x + 2500;
	return x / 2;
}

float offsety(float y)
{
	y = y + 1500;
	return y / 2;
}

Vector2 WorldToScreen(const Vector3 pos, view_matrix_t matrix, float width, float height) {
	float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
	float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];
	float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

	if (w < 0.01f)
		return Vector2(0, 0);

	float inv_w = 1.f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = width * .5f;
	float y = height * .5f;

	x += 0.5f * _x * width + 0.5f;
	y -= 0.5f * _y * height + 0.5f;

	return Vector2(x, y);
}

int main() {
	Vector3 oldPunch = { 0,0,0 };
	while (true)
	{
		//if this is removed it breaks completely
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const uintptr_t localPlayer = VARS::memRead<uintptr_t>(VARS::baseAddress + /*dwLocalPlayer*/ 0x180ACB0);
		if (!localPlayer)
			continue;


		const view_matrix_t view_matrix = VARS::memRead<view_matrix_t>(VARS::baseAddress + 0x1819FD0/*dwViewMatrix*/);

		const uintptr_t entity_list = VARS::memRead<uintptr_t>(VARS::baseAddress + 0x17BB860/*dwEntityList*/);

		const int localTeam = VARS::memRead<int>(localPlayer + 0x3BF/*m_iTeamNum*/);
		
		
		const uintptr_t localPlayerPawn = VARS::memRead<uintptr_t>(VARS::baseAddress + /*dwlocalPlayerPawn*/ 0x16C2DD8);
		
		C_UTL_VECTOR aimPunchCache = VARS::memRead<C_UTL_VECTOR>(localPlayerPawn + 0x1738   /*m_aimPunchCache*/);
		Vector3 viewAngles = VARS::memRead<Vector3>(localPlayerPawn + 0x1510  /*m_angEyeAngles*/);
		Vector3 aimPunchAngle = VARS::memRead<Vector3>(aimPunchCache.data + (aimPunchCache.count - 1) * sizeof(Vector3));
		
		int shots_fired = VARS::memRead<int>(localPlayerPawn + 0x1418 /*m_iShotsFired*/);
		
		if (shots_fired > 1)
		{
			
			//Vector3 recoil = {
			//	viewAngles.x + oldPunch.x - aimPunchAngle.x * 2.0f,
			//	viewAngles.y + oldPunch.y - aimPunchAngle.y * 2.0f,
			//};

			//if (recoil.x > 89.0f && recoil.x <= 180.0f) {
			//	recoil.x = 89.0f;
			//}

			//if (recoil.x > 180.0f) {
			//	recoil.x -= 360.0f;
			//}

			//if (recoil.x < -89.0f) {
			//	recoil.x = -89.0f;
			//}

			//if (recoil.y > 180.0f) {
			//	recoil.y -= 360.0f;
			//}

			//if (recoil.y < -180.0f) {
			//	recoil.y += 360.0f;
			//}
			//recoil.z = 0;

			//VARS::memWrite<Vector3>(VARS::baseAddress + 0x187AC50 /*dwViewAngles*/, recoil);

			Vector3 newAngles = { 0,0,0 };
			newAngles.x = (aimPunchAngle.y - oldPunch.y) * 2.0f / (0.022 * 1.25f);
			newAngles.y = (aimPunchAngle.x - oldPunch.x) * 2.0f / (0.022 * 1.25f);

			mouse_event(MOUSEEVENTF_MOVE, -newAngles.x -1, newAngles.y, 0, 0);
			
			oldPunch.x = aimPunchAngle.x * 1.1f;
			oldPunch.y = aimPunchAngle.y * 1.1f;
			
		}
		else {
			oldPunch.x = 0;
			oldPunch.y = 0;
		}

		
//	}
//}
		int playerIndex = 0;
		uintptr_t list_entry;
		
		while (true) {
			playerIndex++;
			list_entry = VARS::memRead<uintptr_t>(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
			if (!list_entry)
				break;

			const uintptr_t player = VARS::memRead<uintptr_t>(list_entry + 120 * (playerIndex & 0x1FF));
			if (!player)
				continue;

			/**
			* Skip rendering your own character and teammates
			*
			* If you really want you can exclude your own character from the check but
			* since you are in the same team as yourself it will be excluded anyway
			**/
			const int playerTeam = VARS::memRead<int>(player + 0x3BF /*m_iTeamNum*/);
			if (playerTeam == localTeam || playerTeam == 0)
				continue;


			const std::uint32_t playerPawn = VARS::memRead<std::uint32_t>(player + 0x7EC /*m_hPlayerPawn*/);

			const uintptr_t list_entry2 = VARS::memRead<uintptr_t>(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
			if (!list_entry2)
				continue;

			const uintptr_t pCSPlayerPawn /*value*/ = VARS::memRead<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));
			if (!pCSPlayerPawn)
				continue;

			//std::cout << "pCSPlayerPawn: " << pCSPlayerPawn << std::endl;

			const int playerHealth = VARS::memRead<int>(pCSPlayerPawn + 0x32C/*m_iHealth*/);
			//std::cout << "Health: " << playerHealth << " ID: " << playerIndex << std::endl;

			if (playerHealth <= 0 || playerHealth > 100)
				continue;

			if (pCSPlayerPawn == localPlayer)
				continue;

			//const Vector3 origin = VARS::memRead<Vector3>(pCSPlayerPawn + 0x1224/*m_vecOrigin*/);

			//Vector2 pixels = WorldToScreen(origin, view_matrix, width, height);

			uint64_t gamescene = VARS::memRead<uint64_t>(pCSPlayerPawn + 0x310 /*m_pGameSceneNode*/); //0x310: m_iGlobalname, m_nViewModelEffect
			uint64_t bonearray = VARS::memRead<uint64_t>(gamescene + 0x160 /*m_modelState */ + 0x80 /*m_vecOrigin*/); 
			//0x160: m_nDrowningDamageInitial, m_vPrecomputedOBBOrigin, m_modelState, m_vEdgeFadeDists, m_nOpEndCapState, m_nDrowningDamageInitial, m_rogueTimer
			//0x80: m_matrix3x4_t, m_pD3DSemanticName, m_pD3DSemanticName, m_tiles, m_renderable, m_TargetInput, m_bIsPublic, m_vPoint0, m_flLinearDrag, m_qObsInterp_OrientationStart, 
			// m_qObsInterp_OrientationStart, m_vecOrigin, m_pButtonPressedCmdNumber, m_nRoundsPlayedThisPhase, m_nInventoryImageRgbaWidth, m_flAveWindSpeed, m_flValueFloatW_Max

			Vector3 head = VARS::memRead<Vector3>(bonearray + 6 * 32);
			//1,2, 20 = pelvis
			//3,4, 7 = chest
			//5, 12  = neck
			//6 head
			//8 = left shoulder
			//9 = left elbow
			//10,11 = left hand
			//13 = right shoulder
			//14 = right elbow
			//15,16 = right hand
			//17 opposite heart
			//18, 19, 21 heart
			//leftpelvis = 22
			//left knee = 23
			//left foot = 24
			//right pelvis = 25
			//right knee = 26
			//right foot = 27
			//6 -> 5 -> (8->9->10)(13->14->15)(1->(23->24)(26->27))

			
			////playerPawn + m_aimPunchCache as C_UTL_VECTOR (aim punch cache)
			//C_UTL_VECTOR aimPunchCache = VARS::memRead<C_UTL_VECTOR>(pCSPlayerPawn + 0x1738   /*m_aimPunchCache*/);
			//std::cout << "aimPunchCache count: " << aimPunchCache.count << " aimPunchCache data: " << aimPunchCache.data << std::endl;
			////playerPawn + m_angEyeAngles as vector3 (view angles)
			//Vector3 viewAngles = VARS::memRead<Vector3>(pCSPlayerPawn + 0x1510  /*m_angEyeAngles*/);
			//std::cout << "viewAngles: " << viewAngles.x << " " << viewAngles.y << " " << viewAngles.z << std::endl;
			////Vector3 aimPunchAngle = MemMan.ReadMem<Vector3>(localPlayer.aimPunchCache.data + (localPlayer.aimPunchCache.count - 1) * sizeof(Vector3));
			//Vector3 aimPunchAngle = VARS::memRead<Vector3>(aimPunchCache.data + (aimPunchCache.count - 1) * sizeof(Vector3));
			//std::cout << "aimPunchAngle: " << aimPunchAngle.x << " " << aimPunchAngle.y << " " << aimPunchAngle.z << std::endl;

			////playerPawn + m_iShotsFired as int
			////if shots fired > 1
			//int shots_fired = VARS::memRead<int>(VARS::baseAddress + 0x1418 /*m_iShotsFired*/);
			//std::cout << "shots fired: " << shots_fired << std::endl;
			//if (VARS::memRead<int>(pCSPlayerPawn + 0x1418 /*m_iShotsFired*/) > 1)
			//{
			//	std::cout << "shots fired" << std::endl;
			//	Vector3 recoil = {
			//		viewAngles.x + oldPunch.x - aimPunchAngle.x * 2.f,
			//		viewAngles.y + oldPunch.y - aimPunchAngle.y * 2.f,
			//	};

			//	VARS::memWrite<Vector3>(pCSPlayerPawn + 0x187A9D0 /*dwViewAngles*/, recoil);
			//}

			//oldPunch.x = aimPunchAngle.x * 2.f;
			//oldPunch.y = aimPunchAngle.y * 2.f;
			
			//	new_angle = view_angle + old_Punch - new_Punch (each var has .x and on new line .y)
			//else
			//	old angle is 0

			//clamp

			//write new angle to clientbase + dwViewAngles

			//std::cout << "Head: " << head.x << " " << head.y << " " << head.z << std::endl;

			Vector2 pixels = WorldToScreen(head, view_matrix, width, height);

			uintptr_t m_pClippingWeapon = VARS::memRead<uintptr_t>(pCSPlayerPawn + 0x12A8 /*m_pClippingWeapon*/);

			uintptr_t m_pEntity = VARS::memRead<uintptr_t>(m_pClippingWeapon + 0x10 /*m_pEntity*/);

			char buf[32];
			std::cout << VARS::memRead<char>(m_pEntity + 0x20 /*m_designerName*/);
			std::cout << std::endl;

			//Graphics graphics(hdc);
			//Pen pen(Color(255, 255, 0, 0), 3);
			//graphics.DrawLine(&pen, pixels.x, pixels.y, pixels.x+5, pixels.y+1);



		}

	}
}

//#include "memory.h"
//#include <Windows.h>
//#include <iostream>
//#include <thread>
//
//int main() {
//
//	while (true)
//	{
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//		const uintptr_t localPlayer = VARS::memRead<uintptr_t>(VARS::baseAddress + /*dwlocalPlayerPawn*/ 0x16B7340);
//
//		int health = VARS::memRead<int>(localPlayer + 0x32C /*m_ihealth*/);
//
//		//std::cout << "Health: " << (int)health << std::endl;
//
//		int entityId = VARS::memRead<int>(localPlayer + /*m_iIDEntIndex */ 0x153C);
//		
//		std::cout << "ID: " << entityId << std::endl;
//
//		auto crosshair_ent = VARS::memRead<int>(localPlayer + 0x153C /*m_iIDEntIndex*/);
//
//		auto local_player_team = VARS::memRead<int>(localPlayer + 0x3BF/*m_iTeamNum*/);
//
//		if (crosshair_ent != -1)
//		{
//			auto entity_list = VARS::memRead<uintptr_t>(VARS::baseAddress + 0x17AB958/*dwEntityList*/);
//			auto entity_entry = VARS::memRead<uintptr_t>(entity_list + 0x8 * (crosshair_ent >> 0x9) + 0x10);
//
//			auto entity = VARS::memRead<uintptr_t>(entity_entry + 0x78 * (crosshair_ent & 0x1FF));
//			auto entity_team = VARS::memRead<int>(entity + 0x3BF /*m_iTeamNum*/);
//			auto entity_health = VARS::memRead<int>(entity + 0x32C /*m_ihealth*/);
//
//			if (local_player_team != entity_team && entity_health != 0)
//			{
//				std::cout << entity_team << std::endl;
//			}
//		}
//
//
//	}
//}