#include "tools.hpp"
#include <iostream>
#include <map>
#include <thread>
#include "vector3d.hpp"


Vector3D CalculateAngle(Vector3D localPosition, Vector3D enemyPosition, Vector3D viewAngle)
{
	Vector3D vectorBetweenLocalPlayerAndEnemyPlayer = enemyPosition - localPosition;
	return Vector3D::ToAngle(vectorBetweenLocalPlayerAndEnemyPlayer) - viewAngle;
}
 

// Offsets 
static constexpr std::ptrdiff_t dwLocalPlayer = 0xDEA98C;
static constexpr std::ptrdiff_t dwEntityList = 0x4DFFF7C;
static constexpr std::ptrdiff_t dwClientState = 0x59F19C;
static constexpr std::ptrdiff_t m_dwBoneMatrix = 0x26A8;


constexpr ::std::ptrdiff_t m_flFlashDuration = 0x10470;
static constexpr std::ptrdiff_t m_iShotsFired = 0x103E0;
static constexpr std::ptrdiff_t m_bSpottedByMask = 0x980;
static constexpr std::ptrdiff_t m_iHealth = 0x100;
static constexpr std::ptrdiff_t m_bDormant = 0xED;
static constexpr std::ptrdiff_t m_fFlags = 0x104;
static constexpr std::ptrdiff_t dwForceJump = 0x52BBCD8;
constexpr ::std::ptrdiff_t dwForceAttack = 0x322DDE8;

static constexpr std::ptrdiff_t dwGlowObjectManager = 0x535AA08;
static constexpr std::ptrdiff_t m_iTeamNum = 0xF4;
static constexpr std::ptrdiff_t m_iGlowIndex = 0x10488;
static constexpr std::ptrdiff_t m_bSpotted = 0x93D;
static constexpr std::ptrdiff_t m_iCrosshairId = 0x11838;
static constexpr std::ptrdiff_t m_aimPunchAngle = 0x303C;

static constexpr std::ptrdiff_t m_vecOrigin = 0x138;
static constexpr std::ptrdiff_t m_vecViewOffset = 0x108;

// dwClientState
constexpr ::std::ptrdiff_t dwClientState_ViewAngles = 0x4D90;


static HANDLE CONSOLE_HANDLE = nullptr;
static bool CONSOLE_UPDATE = false;

static struct Color
{
	constexpr Color(float r, float g, float b, float a) noexcept : r{ r }, g{ g }, b{ b }, a{ a } {}
	float r, g, b, a;
};

static struct C_CSEntityList
{
	uintptr_t m_baseEntity;
 	int m_teamNum;
	uint32_t m_health;
	bool m_isDormant;
	bool m_isSpottedByMask;
	
	void Update(uint8_t playerID, void* hProcess, std::map<std::string, uint32_t>& modules);

	static constexpr uint8_t EntityLoopDistance = 0x10;
}entityList_t[64];

void C_CSEntityList::Update(uint8_t playerID, void* hProcess, std::map<std::string, uint32_t>& modules)
{
	m_baseEntity = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(modules["client"] + dwEntityList + (playerID * C_CSEntityList::EntityLoopDistance)));
	m_teamNum = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(m_baseEntity + m_iTeamNum));
	m_isDormant = ProcessHack::Get()->ReadProcessMemory<bool>(hProcess, reinterpret_cast<void*>(m_baseEntity + m_bDormant));
	m_health = ProcessHack::Get()->ReadProcessMemory<uint32_t>(hProcess, reinterpret_cast<void*>(m_baseEntity + m_iHealth));
	m_isSpottedByMask = ProcessHack::Get()->ReadProcessMemory<bool>(hProcess, reinterpret_cast<void*>(m_baseEntity + m_isSpottedByMask));
}


static struct C_CSPlayer
{
	uintptr_t m_localPlayer;
	int m_TeamNum; // 1 = spec ; 2 = CT ; 3 = T
	int m_crosshairID;
	int m_shootFired;
	Vector3D m_localPosition;
	Vector3D m_viewOffset;


	static void Update(void* hProcess, std::map<std::string, uint32_t>& modules);
	static void Debug();
}player_t;


void C_CSPlayer::Debug()
{
	std::cout << "struct C_CSPlayer:" << std::endl;
	std::cout << "dwLocalPlayer : " << std::hex << "0x" << player_t.m_localPlayer << std::endl;
	std::cout << "m_iTeamNum : " << std::dec << player_t.m_TeamNum << std::endl;
	std::cout << "m_iCrosshairId : " << std::dec << player_t.m_crosshairID << std::endl;
	std::cout << "m_vecOrigin : " << player_t.m_localPosition << std::endl;
	std::cout << "m_vecViewOffset : " << player_t.m_localPosition << std::endl;

}

void C_CSPlayer::Update(void* hProcess, std::map<std::string, uintptr_t>& modules)
{
	player_t.m_localPlayer = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(modules["client"] + dwLocalPlayer));

	player_t.m_TeamNum = ProcessHack::Get()->ReadProcessMemory<int>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_iTeamNum));
	player_t.m_crosshairID = ProcessHack::Get()->ReadProcessMemory<int>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_iCrosshairId));
	player_t.m_shootFired = ProcessHack::Get()->ReadProcessMemory<int>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_iShotsFired));


	player_t.m_localPosition = ProcessHack::Get()->ReadProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_vecOrigin));
	player_t.m_viewOffset = ProcessHack::Get()->ReadProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_vecViewOffset));
 
	for (auto i = 0; i < 64; i++)
	{
		entityList_t[i].Update(i, hProcess, modules);
	}
}

void bhop(bool activate, void* hProcess, uint32_t clientDLL)
{
	if (!activate)
		return;

 	const auto onGround = ProcessHack::Get()->ReadProcessMemory<int>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_fFlags));

	if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0))
		ProcessHack::Get()->WriteProcessMemory<int>(hProcess, reinterpret_cast<void*>(clientDLL + dwForceJump), 0x6);
}

void glow(bool activate, void* hProcess, uint32_t clientDLL)
{

	if (!activate)
		return;

	auto glowObjectManager = ProcessHack::Get()->ReadProcessMemory<int>(hProcess, reinterpret_cast<void*>(clientDLL + dwGlowObjectManager));

 
	for (auto i = 0; i < 64; i++)
	{
		auto baseEntity = entityList_t[i].m_baseEntity;
		auto entityTeam = entityList_t[i].m_teamNum;

		auto glowIndex = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(baseEntity + m_iGlowIndex));
		
		if (entityTeam == player_t.m_TeamNum)
			continue;

		ProcessHack::Get()->WriteProcessMemory<Color>(hProcess, reinterpret_cast<void*>(glowObjectManager + (glowIndex * 0x38) + 0x8), Color(1.0, 0.0, 0.0, 1.0)); // Red 
		ProcessHack::Get()->WriteProcessMemory<bool>(hProcess, reinterpret_cast<void*>(glowObjectManager + (glowIndex * 0x38) + 0x27), true); // Alpha
		ProcessHack::Get()->WriteProcessMemory<bool>(hProcess, reinterpret_cast<void*>(glowObjectManager + (glowIndex * 0x38) + 0x28), true); // Alpha
	}
}

void radar(bool activate, void* hProcess, uint32_t clientDLL)
{
	if (!activate)
		return;

 
	for (auto i = 0; i < 64; i++)
	{
		auto baseEntity = entityList_t[i].m_baseEntity;
		auto entityTeam = entityList_t[i].m_teamNum;
 
		if (entityTeam == player_t.m_TeamNum)
			continue;
	 
		ProcessHack::Get()->WriteProcessMemory<bool>(hProcess, reinterpret_cast<void*>(baseEntity + m_bSpotted), true);
 	}
}

void triggerbot(bool activate, void* hProcess, uint32_t clientDLL)
{
	if (!activate)
		return;

	if (player_t.m_crosshairID > 0)
	{
		const auto player = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(clientDLL + dwEntityList + (player_t.m_crosshairID - 1) * C_CSEntityList::EntityLoopDistance));
		const auto playerTeamNum = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(player + m_iTeamNum));

		if (playerTeamNum == player_t.m_TeamNum)
			return;
		

		ProcessHack::Get()->WriteProcessMemory<uint32_t>(hProcess, reinterpret_cast<void*>(clientDLL + dwForceAttack), 0x6);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ProcessHack::Get()->WriteProcessMemory<uint32_t>(hProcess, reinterpret_cast<void*>(clientDLL + dwForceAttack), 0x4);
	}
}

void aimbot(bool activate, void* hProcess, uint32_t clientDLL, uint32_t engineDLL)
{
	if (!activate)
		return;

	auto localEyePosition = player_t.m_localPosition + player_t.m_viewOffset;

	auto clientState = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(engineDLL + dwClientState));
	auto viewAngle = ProcessHack::Get()->ReadProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles));
	auto aimPunch = ProcessHack::Get()->ReadProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_aimPunchAngle)) * 2;
	
	auto bestFOV = 5.0f;
	auto bestAngle = Vector3D(0, 0, 0);

	for (auto i = 0; i < 64; i++)
	{
		const auto player = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(clientDLL + dwEntityList + (player_t.m_crosshairID - 1) * C_CSEntityList::EntityLoopDistance));
		const auto playerTeamNum = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(player + m_iTeamNum));
		const auto playerIsDormant = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(player + m_bDormant));
		const auto playerIsMasked = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(player + m_bSpottedByMask));
		const auto playerHealth = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(player + m_iHealth));


		if (playerTeamNum == player_t.m_TeamNum)
			continue;

		if (!playerHealth)
			continue;

		if (playerIsDormant)
			continue;

		if (!playerIsMasked)
			continue;
		
		const auto boneMatrix = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(entityList_t[i].m_baseEntity + m_dwBoneMatrix));

		const auto x = ProcessHack::Get()->ReadProcessMemory<float>(hProcess, reinterpret_cast<void*>(boneMatrix + 0x30 * 0x8 + 0x0C));
		const auto y = ProcessHack::Get()->ReadProcessMemory<float>(hProcess, reinterpret_cast<void*>(boneMatrix + 0x30 * 0x8 + 0x1C));
		const auto z = ProcessHack::Get()->ReadProcessMemory<float>(hProcess, reinterpret_cast<void*>(boneMatrix + 0x30 * 0x8 + 0x2C));
		const auto playerHeadPosition = Vector3D(x, y, z);

		const auto angle = CalculateAngle(localEyePosition, playerHeadPosition, viewAngle + aimPunch);
		const auto fov = std::hypotf(angle.getComponentX(), angle.getComponentY());

		if (fov < bestFOV)
		{
			bestFOV = fov;
			bestAngle = angle;
		}

		if (!Vector3D::IsZero(bestAngle))
			ProcessHack::Get()->WriteProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles), viewAngle + bestAngle * (1/1.5f));
		
 	}

}

void antiaim(bool activate, void* hProcess, uint32_t clientDLL, uint32_t engineDLL)
{
	if (!activate)
		return;
 
	auto clientState = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(engineDLL + dwClientState));
	auto viewAngle = ProcessHack::Get()->ReadProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles));

	viewAngle.setComponentY(viewAngle.getComponentY() * -1.0f);

	ProcessHack::Get()->WriteProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles), viewAngle);
}

void reverse(bool activate, void* hProcess, uint32_t clientDLL, uint32_t engineDLL)
{
	if (!activate)
		return;

	auto clientState = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(engineDLL + dwClientState));
	auto viewAngle = ProcessHack::Get()->ReadProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles));

	viewAngle.setComponentX(-450.f);

	ProcessHack::Get()->WriteProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles), viewAngle);
}

void rcs(bool activate, void* hProcess, uint32_t clientDLL, uint32_t engineDLL)
{
	if (!activate)
		return;


	static Vector2D oldPunch(0, 0);

	if (player_t.m_shootFired)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		auto clientState = ProcessHack::Get()->ReadProcessMemory<uintptr_t>(hProcess, reinterpret_cast<void*>(engineDLL + dwClientState));
		auto aimPunch = ProcessHack::Get()->ReadProcessMemory<Vector2D>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_aimPunchAngle)) ;
		auto viewAngle = ProcessHack::Get()->ReadProcessMemory<Vector3D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles));


		auto newAngle = Vector2D(
			viewAngle.getComponentX() + oldPunch.getComponentX() - aimPunch.getComponentX()* 2.0f,
			viewAngle.getComponentY() + oldPunch.getComponentY() - aimPunch.getComponentY() * 2.0f);


		if (newAngle.getComponentX() > 89.0f)
			newAngle.setComponentX(89.0f);

		if (newAngle.getComponentX() < -89.0f)
			newAngle.setComponentX(89.0f);

		if (newAngle.getComponentY() > -180.f)
			newAngle.setComponentY(newAngle.getComponentY() - 360.0f);

		if (newAngle.getComponentY() < -180.f)
			newAngle.setComponentY(newAngle.getComponentY() + 360.0f);
		 
		ProcessHack::Get()->WriteProcessMemory<Vector2D>(hProcess, reinterpret_cast<void*>(clientState + dwClientState_ViewAngles), newAngle);

		oldPunch.setComponentX(aimPunch.getComponentX() * 2.0f);
		oldPunch.setComponentY(aimPunch.getComponentY() * 2.0f);
	}
	else
	{
		oldPunch.setComponentX(0);
		oldPunch.setComponentY(0);
	}
}

void noflash(bool activate, void* hProcess, uint32_t clientdll)
{
	ProcessHack::Get()->WriteProcessMemory<int>(hProcess, reinterpret_cast<void*>(player_t.m_localPlayer + m_flFlashDuration), 0);
}

void CONSOLE_Menu(const std::map<std::string, bool>& features)
{ 
	SetConsoleTextAttribute(CONSOLE_HANDLE, 15);
	std::cout << "::CSGO External:: - v1.0.0 - 16 July 2023;" << std::endl;
	std::cout << std::endl;

	std::cout << "=================" << std::endl;
	std::cout << "= Features Menu =" << std::endl;
	std::cout << "=================" << std::endl;


	auto i = 0;

	for (const auto& feature : features)
	{
		std::cout << "F" << (i+1) << " - " << feature.first << " ";

		if (!feature.second)
		{
			SetConsoleTextAttribute(CONSOLE_HANDLE, 12);
			std::cout << "[ OFF ]" << std::endl;
		}
		else
		{
			SetConsoleTextAttribute(CONSOLE_HANDLE, 10);
			std::cout << "[ ON ]" << std::endl;
		}
		SetConsoleTextAttribute(CONSOLE_HANDLE, 15);
		i++;
	}
	 
}

void CONSOLE_HandleInputMenu(std::map<std::string, bool>& features)
{
 
	if (GetAsyncKeyState(VK_F1) && (CONSOLE_UPDATE = true))
		features["aimbot"] = !features["aimbot"];
 	if (GetAsyncKeyState(VK_F2) && (CONSOLE_UPDATE = true))
		features["bhop"] = !features["bhop"];
	if (GetAsyncKeyState(VK_F3) && (CONSOLE_UPDATE = true))
		features["glow"] = !features["glow"];
	if (GetAsyncKeyState(VK_F4) && (CONSOLE_UPDATE = true))
		features["radar"] = !features["radar"];
	if (GetAsyncKeyState(VK_F5) && (CONSOLE_UPDATE = true))
		features["rcs"] = !features["rcs"];
	if (GetAsyncKeyState(VK_F6) && (CONSOLE_UPDATE = true))
		features["triggerbot"] = !features["triggerbot"];

	if (CONSOLE_UPDATE)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		system("cls");
		CONSOLE_Menu(features);
		CONSOLE_UPDATE = false;
	}
}

void CONSOLE_PrintError(const std::string msg)
{
	SetConsoleTextAttribute(CONSOLE_HANDLE, 12);
	std::cerr << "[ ERROR ] ";
	SetConsoleTextAttribute(CONSOLE_HANDLE, 15);
	std::cerr << msg << std::endl;
}

void CONSOLE_PrintSuccess(const std::string msg)
{ 
	SetConsoleTextAttribute(CONSOLE_HANDLE, 15);
	std::cerr << msg;
	SetConsoleTextAttribute(CONSOLE_HANDLE, 10);
	std::cerr << " [ OK ] " << std::endl;
	SetConsoleTextAttribute(CONSOLE_HANDLE, 15);

}



int main()
{
	CONSOLE_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
	const auto processID = ProcessHack::Get()->GetProcessIdFromName(L"csgo.exe");

	if (processID == 0)
	{
		CONSOLE_PrintError("Please run Counter-Strike Global Offensive before run the hack !");
		return EXIT_FAILURE;
	}

	const auto hProcess = ProcessHack::Get()->GetProcessHandleFromProcessID(processID);
	const auto clientDLL = ProcessHack::Get()->GetModuleAddressFromProcess(L"client.dll", processID);
	const auto engineDLL = ProcessHack::Get()->GetModuleAddressFromProcess(L"engine.dll", processID);


	CONSOLE_PrintSuccess("Counter-Strike Global Offensive hack injected !");

	std::map<std::string, uint32_t> modules;
	modules["client"] = clientDLL;
	modules["engine"] = engineDLL;

	std::map<std::string, bool> features;
	features["aimbot"] = false;
	features["triggerbot"] = false;
	features["rcs"] = false;
	features["glow"] = false;
	features["bhop"] = false;
	features["radar"] = false;

  

  
 
	CONSOLE_Menu(features);

	while (true)
	{
		 
		CONSOLE_HandleInputMenu(features);


		C_CSPlayer::Update(hProcess, modules);
 

		triggerbot(features["triggerbot"], hProcess, clientDLL);
		aimbot(features["aimbot"], hProcess, clientDLL, engineDLL);
 		glow(features["glow"], hProcess, clientDLL);
		bhop(features["bhop"], hProcess, clientDLL);
		radar(features["radar"], hProcess, clientDLL);
		rcs(features["rcs"], hProcess, clientDLL, engineDLL);
		//noflash(false, hProcess, clientDLL); work but not release to the menu

		 
	
 	}

	return EXIT_SUCCESS;
}