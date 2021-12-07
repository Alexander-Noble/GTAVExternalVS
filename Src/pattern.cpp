#include <Windows.h>
#include <memoryapi.h>
#include "pattern.h"
#include <TlHelp32.h>
#include <stdexcept>
#include <iostream>

char* ScanExternal(char* pattern, char* mask, char* entry, uintptr_t size, HANDLE hProcess) {

	char* pattern_match{ nullptr };
	char* buffer{ nullptr };
	SIZE_T bytes_read_so_far;
	DWORD oldprotection;
	MEMORY_BASIC_INFORMATION mbi;
	mbi.RegionSize = 0x1000;
	char* internal_address;
	VirtualQueryEx(hProcess, (LPCVOID)entry, &mbi, sizeof(mbi));

	for (char* current = entry; current < entry + size; current += mbi.RegionSize) {

		if (!VirtualQueryEx(hProcess, (LPVOID) current, &mbi, sizeof(mbi))) {
			continue;
		}
		if (mbi.Protect == PAGE_NOACCESS || mbi.State != MEM_COMMIT) {
			continue;
		}

		delete[] buffer;
		buffer = new char[mbi.RegionSize];

		if (VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldprotection)) {
			ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, &bytes_read_so_far);
			VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, oldprotection, &oldprotection);

			internal_address = Scan(pattern, mask, buffer, (DWORD) bytes_read_so_far);

			if (internal_address != nullptr) {

				pattern_match = (char*) current + (internal_address - buffer);
				break;
			}

		}
	}
	delete[] buffer;
	return pattern_match;
}

char* Scan(char* pattern, char* mask, char* entry, uintptr_t size) {

	//intptr_t pattern_length = strlen(mask);

	//for (int i = 0; i < size; i++) {

	//	bool pattern_found = true;

	//	for (int j = 0; j < pattern_length; j++) {
	//		//&& mask[j] != '*' && mask[j] != 'x')
	//		if ((mask[j] != '?') && *(char*) ((intptr_t)entry + i + j) != pattern[j]) {
	//			pattern_found = false;
	//			break;
	//			}
	//	}

	//	if (pattern_found) {
	//		return entry + i;
	//	}
	//}

	//return nullptr;
	//	 

	unsigned int patternLength = strlen(mask);

	for (unsigned int i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (unsigned int j = 0; j < patternLength; j++)
		{
			if (mask[j] != '?' && pattern[j] != *(entry + i + j))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return (entry + i);
		}
	}
	return nullptr;
}

char* ScanExternalModule(char* pattern, char* mask, DWORD procID, const wchar_t* modName, HANDLE hProc) {

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(modEntry);

	if (snapshot == INVALID_HANDLE_VALUE) {
		CloseHandle(snapshot);
		throw(std::runtime_error("Invalid handle value"));
	}

	if (Module32First(snapshot, &modEntry)) {

		do {

			if (!_wcsicmp(modName, modEntry.szModule)) {
				break;
			}
		} while (Module32Next(snapshot, &modEntry));
	}
	CloseHandle(snapshot);
	return ScanExternal(pattern, mask, (char*) (modEntry.modBaseAddr), (uintptr_t) modEntry.modBaseSize, hProc);
}