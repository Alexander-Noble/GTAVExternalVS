#pragma once
#include <Windows.h>


char* ScanExternal(char* pattern, char* mask, char* entry, uintptr_t size, HANDLE hProcess);
char* Scan(char* pattern, char* mask, char* entry, uintptr_t size);
char* ScanExternalModule(char* pattern, char* mask, DWORD procID, const wchar_t* modName, HANDLE hProc);