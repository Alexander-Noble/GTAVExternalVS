#include "mem.h"


void mem::Patch(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProc) {
	DWORD oldProtect;
	VirtualProtectEx(hProc, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(hProc, dst, src, size, nullptr);
	VirtualProtectEx(hProc, dst, size, oldProtect, &oldProtect);
}

void mem::Nop(BYTE* dst, unsigned int size, HANDLE hProc) {
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	Patch(dst, nopArray, size, hProc);
	delete[] nopArray;
}
