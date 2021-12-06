#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcID(const wchar_t* procName);
uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modName);
MODULEENTRY32 GetModuleEntry(DWORD procID, const wchar_t* modName);
uintptr_t GetDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<int> offsets);