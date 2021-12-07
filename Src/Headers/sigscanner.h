#pragma once

#include <iostream>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>

struct module {

	DWORD dwBase, dwSize;

};

class SignatureScanner {

public:
	module TargetModule;
	HANDLE TargetHandle;
	DWORD TargetID;


	HANDLE GetProcess(const wchar_t* ProcessName) {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		do {
			if (!_wcsicmp(procEntry.szExeFile, ProcessName)) {

				TargetID = procEntry.th32ProcessID;
				CloseHandle(snapshot);
				TargetHandle = OpenProcess(PROCESS_ALL_ACCESS, false, TargetID);
				return TargetHandle;
			}
		} while (Process32Next(snapshot, &procEntry));

		return NULL;
	}


	module GetModule(const wchar_t* moduleName) {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, TargetID);
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		do {
			if (!_wcsicmp(modEntry.szModule, moduleName)) {
				CloseHandle(snapshot);
				TargetModule = { (DWORD)modEntry.hModule, modEntry.modBaseSize };
				return TargetModule;
			}

		} while (Module32Next(snapshot, &modEntry));

		module failed = { (DWORD)false, (DWORD)false };
		return failed;
	}

	bool CompareMemory(const BYTE* bData, const BYTE* bMask, const char* szMask) {
		for (; *szMask; ++szMask, ++bData, ++bMask) {
			if (*szMask == 'x' && *bData != *bMask) {
				return false;
			}
		}
		return (*szMask == NULL);
	}

	DWORD FindPattern(DWORD start, DWORD size, const char* sig, const char* mask) {

		BYTE* data = new BYTE[size];
		SIZE_T bytes_read_so_far;

		ReadProcessMemory(TargetHandle, (LPVOID)start, data, size, &bytes_read_so_far);

		for (DWORD i = 0; i < size; i++) {

			if (CompareMemory((const BYTE*)(data + i), (const BYTE*)sig, mask)) {
				return start + i;
			}
		}

		delete[] data;
		return NULL;
	}
};

