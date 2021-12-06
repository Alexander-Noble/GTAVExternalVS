#include "proc.h"
#include <stdexcept>


DWORD GetProcID(const wchar_t* procName) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		CloseHandle(snapshot);
		throw(std::runtime_error("Invalid handle value"));
	}

	DWORD procID = 0;
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(procEntry);

	if (Process32First(snapshot, &procEntry)) {

		do {

			if (!_wcsicmp(procName, procEntry.szExeFile)) {
					
				procID = procEntry.th32ProcessID;
				break;
			}

		} while (Process32Next(snapshot, &procEntry));
	}

	CloseHandle(snapshot);
	return procID;
}

uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modName) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
	if (snapshot == INVALID_HANDLE_VALUE) {
		CloseHandle(snapshot);
		throw(std::runtime_error("Invalid handle value"));
	}

	uintptr_t modBase = 0;
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(modEntry);

	if (Module32First(snapshot, &modEntry)) {
			
		do {

			if (!_wcsicmp(modName, modEntry.szModule)) {
					
				modBase = (uintptr_t) modEntry.modBaseAddr;
				break;
			} 
		} while (Module32Next(snapshot, &modEntry));
	}
	CloseHandle(snapshot);
	return modBase;
}

uintptr_t GetDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<int> offsets) {	
	uintptr_t address = ptr;
	
	for (unsigned int i = 0; i < offsets.size(); ++i) {

		ReadProcessMemory(hProc, (BYTE*)address, &address, sizeof(address), 0);
		address += offsets[i];
	}

	return address;
}