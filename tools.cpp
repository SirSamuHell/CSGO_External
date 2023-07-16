#include "tools.hpp"

 
#include <tlhelp32.h>
#include <iostream>

 
ProcessHack::~ProcessHack()
{
	this->Release();
}
void ProcessHack::Release()
{
	for (auto* handle : m_handles)
		CloseHandle(handle);
	m_handles.clear();
}
ProcessHack* ProcessHack::Get()
{
	static ProcessHack process;
	return &process;
}

void* ProcessHack::GetWindowHandle(const std::wstring windowName)
{
	return FindWindowEx(nullptr, nullptr, nullptr, windowName.c_str());
}

int ProcessHack::GetProcessIdFromName(const std::wstring processName)
{
	HANDLE hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapProcess == INVALID_HANDLE_VALUE)
	{
		std::wcerr << "E: connot create snapshot for " << processName << std::endl;
		return -1;
	}

	PROCESSENTRY32 procEntry = {0};
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapProcess, &procEntry);
 
	int processID = 0;
	while (Process32Next(hSnapProcess, &procEntry))
	{
		const std::wstring wStringExeFile = procEntry.szExeFile;

		if (wStringExeFile == processName)
		{
			processID = procEntry.th32ProcessID;
			break;
		}
	}
	
	CloseHandle(hSnapProcess);

	return processID;
}

void* ProcessHack::GetProcessHandleFromProcessID(const int processID)
{
	void* hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);

	if (hProcess == INVALID_HANDLE_VALUE)
	{
		std::cerr << "E: Connot open the process PID=" << processID << std::endl;
		return nullptr;
	}

	m_handles.push_back(hProcess);

 	return hProcess;
}

uint32_t ProcessHack::GetModuleAddressFromProcess(const std::wstring targetModule, const int processID)
{
	HANDLE hSnapModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);

	if (hSnapModule == INVALID_HANDLE_VALUE)
	{
		std::cerr << "E: coonot create snapshot for the PID=" << processID << std::endl;
		return 0;
	}

	MODULEENTRY32 modEntry = {0};
	modEntry.dwSize = sizeof(MODULEENTRY32);

	Module32First(hSnapModule, &modEntry);

	uint8_t* moduleBaseAddress = nullptr;

	while (Module32Next(hSnapModule, &modEntry))
	{
		std::wstring modName = modEntry.szModule;
		if (modName == targetModule)
		{
			moduleBaseAddress = modEntry.modBaseAddr;
		}
	}

	CloseHandle(hSnapModule);

	return reinterpret_cast<uint32_t>(moduleBaseAddress);
}

std::vector<std::wstring> ProcessHack::GetModuleFilenameFromProcessID(const int processID)
{
 
	HANDLE hSnapModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);

	if (hSnapModule == INVALID_HANDLE_VALUE)
	{
		std::cerr << "E: coonot create snapshot for the PID=" << processID << std::endl;
		return {};
 	}

 
	MODULEENTRY32 modEntry = {0};
	modEntry.dwSize = sizeof(MODULEENTRY32);
	Module32First(hSnapModule, &modEntry);

	std::vector<std::wstring> names;
	while (Module32Next(hSnapModule, &modEntry))
	{
		names.push_back(modEntry.szExePath);
	}

	CloseHandle(hSnapModule);

	return names;
}
 