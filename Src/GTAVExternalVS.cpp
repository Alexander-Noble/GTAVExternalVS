// GTAVExternalVS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "proc.h"
#include "mem.h"
#include "pattern.h"
#include "sigs.h"
#include "sigscanner.h"


int main()
{
    
    HANDLE hProc = 0;
    uintptr_t modBase = 0, PlayerPtr = 0, worldPtr = 0;
    uintptr_t sprintAddr = 0, godAddr = 0, wantedAddr = 0, vehicleGodAddr = 0, spreadAddr = 0, recoilAddr = 0;
    char* clipAddr;
    char* abilityAddr;
    bool bClip = false;
    bool bGod = false;
    bool bAbility = false;
    bool bVehGod = false;
    bool bSpread = false;
    bool bRecoil = false;

    std::vector<int> sprint_speed_offset = { 0x08, 0x10B8, 0x14C };
    std::vector<int> god_mode_offset = { 0x08, 0x189 };
    std::vector<int> wanted_level_offset = { 0x08, 0x10B8, 0x0848 };
    std::vector<int> recoil_offset = { 0x08, 0x10C8, 0x0020, 0x02E8 };
    std::vector<int> spread_offset = { 0x08, 0x10C8, 0x0020, 0x74 };
    std::vector<int> vehicle_god_offset = { 0x08, 0x0D28, 0x0189 };

    DWORD procID = GetProcID(L"GTA5.exe");

    if (procID) {

        hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);
        modBase = GetModuleBaseAddress(procID, L"GTA5.exe");
        worldPtr = modBase + 0x24ACB48;
        PlayerPtr = modBase + 0x08;
        clipAddr = ScanExternalModule(const_cast<char*>(clipPattern), const_cast<char*>(clipMask), procID, L"GTA5.exe", hProc) + 16; 
        abilityAddr = ScanExternalModule(const_cast<char*>(abilityPattern), const_cast<char*>(abilityMask), procID, L"GTA5.exe", hProc);
        sprintAddr = GetDMAAddy(hProc, worldPtr, sprint_speed_offset);
        godAddr = GetDMAAddy(hProc, worldPtr, god_mode_offset);
        wantedAddr = GetDMAAddy(hProc, worldPtr, wanted_level_offset);
        vehicleGodAddr = GetDMAAddy(hProc, worldPtr, vehicle_god_offset);
        spreadAddr = GetDMAAddy(hProc, worldPtr, spread_offset);
        recoilAddr = GetDMAAddy(hProc, worldPtr, recoil_offset);
    }

    else {
        std::cout << "Process not found" << std::endl;
        system("pause");
        return 0;
    }

    DWORD dwExit = 0;

    while (GetExitCodeProcess(hProc, &dwExit) && dwExit == STILL_ACTIVE) {
        bool update_console = true;
        if (update_console) {
            system("cls");
            std::cout << "[NUMPAD 1] God Mode: " << (bGod ? "ON" : "OFF") << std::endl;
            std::cout << "[NUMPAD 2] Infinite Ability: " << (bAbility ? "ON" : "OFF") << std::endl;
            std::cout << "[NUMPAD 3] No Spread: " << (bSpread ? "ON" : "OFF") << std::endl;
            std::cout << "[NUMPAD 4] No Recoil: " << (bRecoil? "ON" : "OFF") << std::endl;
            std::cout << "[END] Vehicle God Mode: " << (bVehGod ? "ON" : "OFF") << std::endl;
            std::cout << "[DEL] Set Wanted Level " << std::endl;
            std::cout << "[HOME] Infinite Clip: " << (bClip ? "ON" : "OFF") << std::endl;
            
            update_console = !update_console;
        }
        if (GetAsyncKeyState(VK_NUMPAD1) & 1) {

            if (!bGod) {
                bGod = !bGod;
                BYTE val = 1;
                mem::Patch((BYTE*) godAddr, &val, sizeof(val), hProc);
                update_console = !update_console;
            }
            else {
                bGod = !bGod;
                BYTE val = 0;
                mem::Patch((BYTE*)godAddr, &val, sizeof(val), hProc);
                update_console = !update_console;
            }
        
        }

        if (GetAsyncKeyState(VK_NUMPAD2) & 1) {

            if (!bAbility) {
                bAbility = !bAbility;
                mem::Nop((BYTE*)abilityAddr, 4, hProc);
                update_console = !update_console;
            }
            else {
                bAbility = !bAbility;
                mem::Patch((BYTE*)abilityPattern, (BYTE*)&abilityPattern, sizeof(abilityPattern), hProc);
                update_console = !update_console;
            }
            
        }

        if (GetAsyncKeyState(VK_NUMPAD3) & 1) {

            if (!bSpread) {
                bSpread = !bSpread;
                float spread = 0;
                mem::Patch((BYTE*)spreadAddr, (BYTE*)&spread, sizeof(spread), hProc);
                update_console = !update_console;
            }
            else {
                bSpread = !bSpread;
                float spread = 3;
                mem::Patch((BYTE*)spreadAddr, (BYTE*)&spread, sizeof(spread), hProc);
                update_console = !update_console;
            }
        }

        if (GetAsyncKeyState(VK_NUMPAD4) & 1) {

            if (!bRecoil) {
                bRecoil = !bRecoil;
                float recoil = 0;
                mem::Patch((BYTE*)recoilAddr, (BYTE*)&recoil, sizeof(recoil), hProc);
                update_console = !update_console;
            }
            else {
                bRecoil = !bRecoil;
                float recoil = 3;
                mem::Patch((BYTE*)recoilAddr, (BYTE*)&recoil, sizeof(recoil), hProc);
                update_console = !update_console;
            }
        }

        if (GetAsyncKeyState(VK_END) & 1) {
            
            if (!bVehGod) {
                bVehGod = !bVehGod;
                BYTE val = 1;
                mem::Patch((BYTE*)vehicleGodAddr, &val, sizeof(val), hProc);
                update_console = !update_console;
            }
            else {
                bVehGod = !bVehGod;
                BYTE val = 0;
                mem::Patch((BYTE*)vehicleGodAddr, &val, sizeof(val), hProc);
                update_console = !update_console;
            }
        }

        if (GetAsyncKeyState(VK_HOME) & 1) {
            if (!bClip) {
                bClip = !bClip;
                int reduction = 0;
                mem::Patch((BYTE*)clipAddr, (BYTE*)&reduction, sizeof(reduction), hProc);
                update_console = !update_console;
            }
            else {
                bClip = !bClip;
                int reduction = 2;
                mem::Patch((BYTE*)clipAddr, (BYTE*)&reduction, sizeof(reduction), hProc);
                update_console = !update_console;
            }


        }

        if (GetAsyncKeyState(VK_DELETE) & 1) {
            int32_t level = 0;
            std::cout << "Please enter a wanted level..." << std::endl;
            std::cin >> level;
            mem::Patch((BYTE*)wantedAddr, (BYTE*)&level, sizeof(level), hProc);
            update_console = !update_console;
        }

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            break;
        }



    }

    CloseHandle(hProc);
    return 0;


    





}
