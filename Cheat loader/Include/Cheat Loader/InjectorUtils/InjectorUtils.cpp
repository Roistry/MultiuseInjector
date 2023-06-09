#include "InjectorUtils.h"

DWORD GetPID(char* processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    Process32First(hSnapshot, &pe32);
    do
        if (strcmp(pe32.szExeFile, processName) == 0)
        {
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID;
        }
    while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    return 0;
}

char* GetProcessName(DWORD pID)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    Process32First(hSnapshot, &pe32);
    do
        if (pe32.th32ProcessID == pID)
        {
            CloseHandle(hSnapshot);
            return pe32.szExeFile;
        }
    while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    return nullptr;
}

DWORD IsWow64(DWORD pID)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pID);
    if (!hProcess)
        return 0;
    BOOL bIsWow64;
    if (!IsWow64Process(hProcess, &bIsWow64))
        return 0;
    CloseHandle(hProcess);
    if (bIsWow64 == 0)
        return x64Process;
    else
        return x86Process;
}

bool ValidFile(char* fileName)
{
    std::ifstream stream(fileName);
    return stream.good();
}

bool RemoveFile(char* fileName)
{
    if (remove(fileName))
        return false;
    return true;
}

char* combineChars(char* str1, char* str2)
{
    char* newStr = new char[strlen(str1) + strlen(str2) + 1];
    strcpy_s(newStr, strlen(str1) + 1, str1);
    strcpy_s(strchr(newStr, '\0'), strlen(str2) + 1, str2);
    return newStr;
}

DWORD x86GetPEB(HANDLE hProcess)
{
	if (!oNtQueryInformationProcess)
	{
		HMODULE hModuleNtdll = LoadLibraryA("ntdll.dll");
		if (hModuleNtdll)
			oNtQueryInformationProcess = (tNtQueryInformationProcess)GetProcAddress(hModuleNtdll, "NtQueryInformationProcess");
	}

	if (oNtQueryInformationProcess)
	{
		PROCESS_BASIC_INFORMATION pbi{ 0 };
		oNtQueryInformationProcess(hProcess, PROCESSINFOCLASS::ProcessBasicInformation, &pbi, sizeof(pbi), nullptr);
		return (DWORD)pbi.PebBaseAddress + 0x1000;
	}
	return 0;
}

PEB x64GetPEB(HANDLE hProcess)
{
	if (!oNtQueryInformationProcess)
	{
		HMODULE hModuleNtdll = LoadLibraryA("ntdll.dll");
		if (hModuleNtdll)
			oNtQueryInformationProcess = (tNtQueryInformationProcess)GetProcAddress(hModuleNtdll, "NtQueryInformationProcess");
	}

	if (oNtQueryInformationProcess)
	{
		PROCESS_BASIC_INFORMATION pbi{ 0 };
		oNtQueryInformationProcess(hProcess, PROCESSINFOCLASS::ProcessBasicInformation, &pbi, sizeof(pbi), nullptr);
		if (pbi.PebBaseAddress)
		{
			PEB peb;
			ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr);
			return peb;
		}
	}
}

DWORD GetProcessIDByName(const char* processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    Process32First(hSnapshot, &pe32);
    do
        if (strcmp(pe32.szExeFile, processName) == 0)
        {
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID;
        }
    while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    return 0;
}

HANDLE GetHandleProcessByName(const char* processName, DWORD dwDesiredAccess)
{
    DWORD pid = GetProcessIDByName(processName);
    if (!pid)
        return nullptr;

    return OpenProcess(dwDesiredAccess, false, pid);
}

void* x64PEBGetModuleHandle(const char* processName, const wchar_t* moduleName)
{
    HANDLE hProcess = GetHandleProcessByName(processName, PROCESS_ALL_ACCESS);
    PEB peb = x64GetPEB(hProcess);
    CloseHandle(hProcess);

    for (LIST_ENTRY* currentModuleEntry = peb.Ldr->InLoadOrderModuleList.Flink; currentModuleEntry != &peb.Ldr->InLoadOrderModuleList; currentModuleEntry = currentModuleEntry->Flink)
    {
        LDR_DATA_TABLE_ENTRY* currentModule = CONTAINING_RECORD(currentModuleEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        if (wcscmp(moduleName, currentModule->BaseDllName.Buffer) == 0)
            return currentModule->DllBase;
    }
    return nullptr;
}

void* x64PEBGetModuleHandle(HANDLE hProcess, const wchar_t* moduleName)
{
	PEB peb = x64GetPEB(hProcess);

	for (LIST_ENTRY* currentModuleEntry = peb.Ldr->InLoadOrderModuleList.Flink; currentModuleEntry != &peb.Ldr->InLoadOrderModuleList; currentModuleEntry = currentModuleEntry->Flink)
	{
		LDR_DATA_TABLE_ENTRY* currentModule = CONTAINING_RECORD(currentModuleEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (wcscmp(moduleName, currentModule->BaseDllName.Buffer) == 0)
			return currentModule->DllBase;
	}
	return nullptr;
}

void* x86PEBGetModuleHandle(const char* processName, const wchar_t* moduleName)
{
	HANDLE hProcess = GetHandleProcessByName(processName, PROCESS_ALL_ACCESS);
	DWORD pebAddress = x86GetPEB(hProcess);

	DWORD ldr{ 0 };
	ReadProcessMemory(hProcess, (void*)(pebAddress + 0xC), &ldr, sizeof(ldr), nullptr);

	DWORD tail{ 0 };
	ReadProcessMemory(hProcess, (void*)(ldr + 0xC), &tail, sizeof(tail), nullptr);
	tail += 0x8;

	DWORD dllBase{ 0 };
	DWORD currentModuleEntry{ 0 };
	DWORD BufferPtr{ 0 };
	wchar_t Buffer[MAX_PATH]{ 0 };
	for (ReadProcessMemory(hProcess, (void*)(tail), &currentModuleEntry, sizeof(currentModuleEntry), nullptr); currentModuleEntry != tail; ReadProcessMemory(hProcess, (void*)(currentModuleEntry), &currentModuleEntry, sizeof(currentModuleEntry), nullptr))
	{
		ReadProcessMemory(hProcess, (void*)(currentModuleEntry + 0x28), &BufferPtr, sizeof(BufferPtr), nullptr);
		ReadProcessMemory(hProcess, (void*)(BufferPtr), &Buffer, sizeof(Buffer), nullptr);
		if (wcscmp(moduleName, Buffer) == 0)
		{
			ReadProcessMemory(hProcess, (void*)(currentModuleEntry + 0x10), &dllBase, sizeof(dllBase), nullptr);
			break;
		}
	}
	CloseHandle(hProcess);
	return (void*)(dllBase);
}

void* x86PEBGetModuleHandle(HANDLE hProcess, const wchar_t* moduleName)
{
	DWORD pebAddress = x86GetPEB(hProcess);

	DWORD ldr{ 0 };
	ReadProcessMemory(hProcess, (void*)(pebAddress + 0xC), &ldr, sizeof(ldr), nullptr);

	DWORD tail{ 0 };
	ReadProcessMemory(hProcess, (void*)(ldr + 0xC), &tail, sizeof(tail), nullptr);
	tail += 0x8;

	DWORD dllBase{ 0 };
	DWORD currentModuleEntry{ 0 };
	DWORD BufferPtr{ 0 };
	wchar_t Buffer[MAX_PATH]{ 0 };
	for (ReadProcessMemory(hProcess, (void*)(tail), &currentModuleEntry, sizeof(currentModuleEntry), nullptr); currentModuleEntry != tail; ReadProcessMemory(hProcess, (void*)(currentModuleEntry), &currentModuleEntry, sizeof(currentModuleEntry), nullptr))
	{
		ReadProcessMemory(hProcess, (void*)(currentModuleEntry + 0x28), &BufferPtr, sizeof(BufferPtr), nullptr);
		ReadProcessMemory(hProcess, (void*)(BufferPtr), &Buffer, sizeof(Buffer), nullptr);
		if (wcscmp(moduleName, Buffer) == 0)
		{
			ReadProcessMemory(hProcess, (void*)(currentModuleEntry + 0x10), &dllBase, sizeof(dllBase), nullptr);
			break;
		}
	}
	return (void*)(dllBase);
}

DWORD x86NtCreateThreadEx(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress)
{
	oNtCreateThreadEx = (tNtCreateThreadEx)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtCreateThreadEx");
	void* allocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	HANDLE hThread = nullptr;
	if (oNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, nullptr, hProcess, (PUSER_THREAD_START_ROUTINE)(fRoutine), arg, 0, 0, 0, 0, nullptr) < 0)
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "NtCreateThreadEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (!hThread)
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "Thread handle returned by NtCreateThreadEx is 0", "ShellCode Injection", MB_OK);
		return 0;
	}

	DWORD dwWaitRet = WaitForSingleObject(hThread, 5000);
	if (dwWaitRet != WAIT_OBJECT_0)
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "Thread didnt create after 5 seconds", "ShellCode Injection", MB_OK);
		return 0;
	}

	DWORD dwRemoteRet = 0;
	if (!GetExitCodeThread(hThread, &dwRemoteRet))
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		CloseHandle(hThread);
		MessageBoxA(0, "Couldnt get exit code of thread", "ShellCode Injection", MB_OK);
		return 0;
	}

	CloseHandle(hThread);

	remoteAddress = dwRemoteRet;

	VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
	return 1;
}

DWORD x86HijackThread(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress)
{
	Sleep(20); //if not sleeping it will crash for some dumbass stupid ass reason.

	DWORD pID = GetProcessId(hProcess);
	if (!pID)
	{
		MessageBoxA(0, "GetProcessId failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pID);
	if (!hSnapshot)
	{
		MessageBoxA(0, "CreateToolhelp32Snapshot failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	THREADENTRY32 te32;
	te32.dwSize = sizeof(te32);

	DWORD tID;
	Thread32First(hSnapshot, &te32);
	do
		if (te32.th32OwnerProcessID == pID)
		{
			tID = te32.th32ThreadID;
			break;
		}
	while (Thread32Next(hSnapshot, &te32));
	CloseHandle(hSnapshot);

	if (!tID)
	{
		MessageBoxA(0, "Couldnt find an obtainable tID", "ShellCode Injection", MB_OK);
		return 0;
	}

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, tID);
	if (!hThread)
	{
		MessageBoxA(0, "Couldnt open handle to tID", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (SuspendThread(hThread) == (DWORD)-1)
	{
		CloseHandle(hThread);
		MessageBoxA(0, "SuspendThread failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	WOW64_CONTEXT oldTC;
	oldTC.ContextFlags = CONTEXT_CONTROL;

	if (!Wow64GetThreadContext(hThread, &oldTC))
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		MessageBoxA(0, "Wow64GetThreadContext failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	void* allocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!hThread)
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	BYTE Shellcode[] =
	{
			0x00, 0x00, 0x00, 0x00,                     // - 0x04 (pCodecave)   -> returned value                           ;buffer to store returned value (eax)

			0x83, 0xEC, 0x04,                          // + 0x00               -> sub esp, 0x04                           ;prepare stack for ret
			0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00,   // + 0x03 (+ 0x06)      -> mov [esp], OldEip                        ;store old eip as return address

			0x50, 0x51, 0x52,                           // + 0x0A               -> psuh e(a/c/d)                            ;save e(a/c/d)x
			0x9C,                                       // + 0x0D               -> pushfd                                   ;save flags register

			0xB9, 0x00, 0x00, 0x00, 0x00,               // + 0x0E (+ 0x0F)     -> mov ecx, pArg                            ;load pArg into ecx
			0xB8, 0x00, 0x00, 0x00, 0x00,               // + 0x13 (+ 0x14)      -> mov eax, pRoutine

			0x51,                                       // + 0x18               -> push ecx                                 ;push pArg
			0xFF, 0xD0,                                 // + 0x19               -> call eax                                 ;call target function

			0xA3, 0x00, 0x00, 0x00, 0x00,               // + 0x1B (+ 0x1C)     -> mov dword ptr[pCodecave], eax            ;store returned value

			0x9D,                                       // + 0x20               -> popfd                                    ;restore flags register
			0x5A, 0x59, 0x58,                           // + 0x21               -> pop e(d/c/a)                             ;restore e(d/c/a)x

			0xC6, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,   // + 0x24 (+ 0x26)      -> mov byte ptr[pCodecave + 0x06], 0x00     ;set checkbyte to 0

			0xC3                                       // + 0x2B               -> ret                                     ;return to OldEip

	}; // SIZE = 0x2C (+ 0x04)

	DWORD dwEIP = (DWORD)(oldTC.Eip & 0xFFFFFFFF);

	*(DWORD*)(Shellcode + 10) = dwEIP;

	*(void**)(Shellcode + 19) = arg;
	*(void**)(Shellcode + 24) = fRoutine;

	*(void**)(Shellcode + 32) = allocationAddress;
	*(BYTE**)(Shellcode + 42) = (BYTE*)((BYTE*)allocationAddress + 6);

	oldTC.Eip = (DWORD)(allocationAddress)+4;

	if (!WriteProcessMemory(hProcess, allocationAddress, Shellcode, sizeof(Shellcode), nullptr))
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "WriteProcessMemory failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (!Wow64SetThreadContext(hThread, &oldTC))
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "Wow64SetThreadContext failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (ResumeThread(hThread) == (DWORD)-1)
	{
		CloseHandle(hThread);
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "ResumeThread failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	CloseHandle(hThread);

	DWORD timer = GetTickCount();
	BYTE checkByte = 1;

	do
	{
		ReadProcessMemory(hProcess, (void*)((BYTE*)allocationAddress + 6), &checkByte, sizeof(checkByte), nullptr);
		if (GetTickCount() - timer > 5000)
			break;

		Sleep(10);
	} while (checkByte != 0);

	ReadProcessMemory(hProcess, allocationAddress, &remoteAddress, sizeof(remoteAddress), nullptr);

	VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
	return 1;
}

DWORD x86SetWindowHookEx(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress)
{
	void* pAllocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pAllocationAddress)
	{
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	HMODULE hModuleUser32 = (HMODULE)(x86PEBGetModuleHandle(hProcess, L"User32.dll"));

	DWORD pCallNextHookEx = (DWORD)((uintptr_t)(hModuleUser32) + 0x2D470);

	BYTE Shellcode[] =
	{
			0x00, 0x00, 0x00, 0x00,         // - 0x08               -> pArg                     ;pointer to argument
			0x00, 0x00, 0x00, 0x00,         // - 0x04               -> pRoutine                 ;pointer to target function

			0x55,                           // + 0x00               -> push ebp                 ;x86 stack frame creation
			0x8B, 0xEC,                     // + 0x01               -> mov ebp, esp

			0xFF, 0x75, 0x10,               // + 0x03               -> push [ebp + 0x10]        ;push CallNextHookEx arguments
			0xFF, 0x75, 0x0C,               // + 0x06               -> push [ebp + 0x0C]
			0xFF, 0x75, 0x08,               // + 0x09               -> push [ebp + 0x08]
			0x6A, 0x00,                     // + 0x0C               -> push 0x00

			0xE8, 0x00, 0x00, 0x00, 0x00,   // + 0x0E (+ 0x0F)      -> call CallNextHookEx      ;call CallNextHookEx

			0xEB, 0x00,                     // + 0x13               -> jmp $ + 0x02             ;jmp to next instruction

			0x50,                           // + 0x15               -> push eax                 ;save eax (CallNextHookEx retval)
			0x53,                           // + 0x16               -> push ebx                 ;save ebx (non volatile)

			0xBB, 0x00, 0x00, 0x00, 0x00,   // + 0x17 (+ 0x18)      -> mov ebx, pArg            ;move pArg (pCodecave) into ebx
			0xC6, 0x43, 0x1C, 0x14,         // + 0x1C               -> mov [ebx + 0x1C], 0x17   ;hotpatch jmp above to skip shellcode

			0xFF, 0x33,                     // + 0x20               -> push [ebx]               ;push pArg (__stdcall)

			0xFF, 0x53, 0x04,               // + 0x22               -> call [ebx + 0x04]        ;call target function

			0x89, 0x03,                     // + 0x25               -> mov [ebx], eax           ;store returned value

			0x5B,                           // + 0x27               -> pop ebx                  ;restore old ebx
			0x58,                           // + 0x28               -> pop eax                  ;restore eax (CallNextHookEx retval)
			0x5D,                           // + 0x29               -> pop ebp                  ;restore ebp
			0xC2, 0x0C, 0x00                // + 0x2A               -> ret 0x000C               ;return
	}; // SIZE = 0x3D (+ 0x08)

	*(DWORD*)(Shellcode + 0) = (DWORD)(arg);
	*(DWORD*)(Shellcode + 4) = (DWORD)(fRoutine); 
	*(DWORD*)(Shellcode + 23) = pCallNextHookEx - ((DWORD)(pAllocationAddress) + 22) - 5;
	*(DWORD*)(Shellcode + 32) = (DWORD)(pAllocationAddress);



	return 1;
}

DWORD x86QueueUserAPC(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress)
{
	void* pAllocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pAllocationAddress)
	{
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	BYTE Shellcode[] =
	{
			0x00, 0x00, 0x00, 0x00, // - 0x0C   -> returned value                   ;buffer to store returned value
			0x00, 0x00, 0x00, 0x00, // - 0x08   -> pArg                             ;buffer to store argument
			0x00, 0x00, 0x00, 0x00, // - 0x04   -> pRoutine                         ;pointer to the routine to call

			0x55,                   // + 0x00   -> push ebp                         ;x86 stack frame creation
			0x8B, 0xEC,             // + 0x01   -> mov ebp, esp

			0xEB, 0x00,             // + 0x03   -> jmp pCodecave + 0x05 (+ 0x0C)    ;jump to next instruction

			0x53,                   // + 0x05   -> push ebx                         ;save ebx
			0x8B, 0x5D, 0x08,       // + 0x06   -> mov ebx, [ebp + 0x08]            ;move pCodecave into ebx (non volatile)

			0xFF, 0x73, 0x04,       // + 0x09   -> push [ebx + 0x04]                ;push pArg on stack
			0xFF, 0x53, 0x08,       // + 0x0C   -> call dword ptr[ebx + 0x08]       ;call pRoutine

			0x85, 0xC0,             // + 0x0F   -> test eax, eax                    ;check if eax indicates success/failure
			0x74, 0x06,             // + 0x11   -> je pCodecave + 0x19 (+ 0x0C)     ;jmp to cleanup if routine failed

			0x89, 0x03,             // + 0x13   -> mov [ebx], eax                   ;store returned value
			0xC6, 0x43, 0x10, 0x15, // + 0x15   -> mov byte ptr [ebx + 0x10], 0x15  ;hot patch jump to skip shellcode

			0x5B,                   // + 0x19   -> pop ebx                          ;restore old ebx
			0x5D,                   // + 0x1A   -> pop ebp                          ;restore ebp

			0xC2, 0x04, 0x00        // + 0x1B   -> ret 0x0004                       ;return
	}; // SIZE = 0x1E (+ 0x0C)

	*(DWORD*)(Shellcode + 4) = (DWORD)(arg);
	*(DWORD*)(Shellcode + 8) = (DWORD)(fRoutine);

	if (!WriteProcessMemory(hProcess, pAllocationAddress, Shellcode, sizeof(Shellcode), nullptr))
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "WriteProcessMemory failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	DWORD PID = GetProcessId(hProcess);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, PID);
	if (!hSnapshot)
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	THREADENTRY32 te32{ 0 };
	te32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hSnapshot, &te32))
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		CloseHandle(hSnapshot);
		MessageBoxA(0, "Thread32First failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	bool bAPCQueued = false;
	do
	{
		if (te32.th32OwnerProcessID != PID)
			continue;

		HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, te32.th32ThreadID);
		if (!hThread)
			continue;

		if (QueueUserAPC((PAPCFUNC)((BYTE*)(pAllocationAddress) + 12), hThread, (ULONG_PTR)(pAllocationAddress)))
			bAPCQueued = true;

		CloseHandle(hThread);
	} while (Thread32Next(hSnapshot, &te32));

	CloseHandle(hSnapshot);

	if (!bAPCQueued)
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "Thread32First failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	DWORD dwTimer = GetTickCount();

	do
	{
		ReadProcessMemory(hProcess, pAllocationAddress, &remoteAddress, sizeof(remoteAddress), nullptr);
		if (GetTickCount() - dwTimer > 5000)
		{
			MessageBoxA(0, "Couldnt get return address of remote address", "ShellCode Injection", MB_OK);
			return 0;
		}
		Sleep(10);
	} while (!remoteAddress);

	return 1;
}

DWORD x86StartRoutine(HANDLE hProcess, LaunchMethod launchMethod, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress)
{
	DWORD dwRet = 0;
	switch (launchMethod)
	{
	case NTCREATETHREAD:
		dwRet = x86NtCreateThreadEx(hProcess, fRoutine, arg, remoteAddress);
		break;
	case HIJACKTHREAD:
		dwRet = x86HijackThread(hProcess, fRoutine, arg, remoteAddress);
		break;
	case SETWINDOWHOOKEX:
		dwRet = x86SetWindowHookEx(hProcess, fRoutine, arg, remoteAddress);
		break;
	case QUEUEUSERAPC:
		dwRet = x86QueueUserAPC(hProcess, fRoutine, arg, remoteAddress);
		break;
	}
	return dwRet;
}



DWORD x64NtCreateThreadEx(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress)
{
	oNtCreateThreadEx = (tNtCreateThreadEx)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtCreateThreadEx");
	void* allocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	BYTE Shellcode[] =
	{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // - 0x10   -> argument / returned value
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // - 0x08   -> pRoutine

			0x48, 0x8B, 0xC1,                                   // + 0x00   -> mov rax, rcx
			0x48, 0x8B, 0x08,                                   // + 0x03   -> mov rcx, [rax]

			0x48, 0x83, 0xEC, 0x28,                             // + 0x06   -> sub rsp, 0x28
			0xFF, 0x50, 0x08,                                   // + 0x0A   -> call qword ptr [rax + 0x08]
			0x48, 0x83, 0xC4, 0x28,                             // + 0x0D   -> add rsp, 0x28

			0x48, 0x8D, 0x0D, 0xD8, 0xFF, 0xFF, 0xFF,           // + 0x11   -> lea rcx, [pCodecave]
			0x48, 0x89, 0x01,                                   // + 0x18   -> mov [rcx], rax
			0x48, 0x31, 0xC0,                                   // + 0x1B   -> xor rax, rax

			0xC3                                                // + 0x1E   -> ret
	}; // SIZE = 0x1F (+ 0x10)

	*(void**)Shellcode = arg;
	*(x64tRoutine**)(Shellcode + 8) = fRoutine;

	if (!WriteProcessMemory(hProcess, allocationAddress, Shellcode, sizeof(Shellcode), nullptr))
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "WriteProcessMemory failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	HANDLE hThread = nullptr;
	if (oNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, nullptr, hProcess, (PUSER_THREAD_START_ROUTINE)((uintptr_t)allocationAddress + 0x10), allocationAddress, 0, 0, 0, 0, nullptr) < 0)
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "NtCreateThreadEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (!hThread)
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "Thread handle returned by NtCreateThreadEx is 0", "ShellCode Injection", MB_OK);
		return 0;
	}

	DWORD dwWaitRet = WaitForSingleObject(hThread, 5000);
	if (dwWaitRet != WAIT_OBJECT_0)
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "Thread didnt create after 5 seconds", "ShellCode Injection", MB_OK);
		return 0;
	}

	CloseHandle(hThread);

	if (!ReadProcessMemory(hProcess, allocationAddress, &remoteAddress, sizeof(remoteAddress), nullptr))
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "ReadProcessMemory failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
	return 1;
}

DWORD x64HijackThread(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress)
{
	Sleep(20); //if not sleeping it will crash for some dumbass stupid ass reason.

	DWORD pID = GetProcessId(hProcess);
	if (!pID)
	{
		MessageBoxA(0, "GetProcessId failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pID);
	if (!hSnapshot)
	{
		MessageBoxA(0, "CreateToolhelp32Snapshot failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	THREADENTRY32 te32;
	te32.dwSize = sizeof(te32);

	DWORD tID;
	Thread32First(hSnapshot, &te32);
	do
		if (te32.th32OwnerProcessID == pID)
		{
			tID = te32.th32ThreadID;
			break;
		}
	while (Thread32Next(hSnapshot, &te32));
	CloseHandle(hSnapshot);

	if (!tID)
	{
		MessageBoxA(0, "Couldnt find an obtainable tID", "ShellCode Injection", MB_OK);
		return 0;
	}

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, tID);
	if (!hThread)
	{
		MessageBoxA(0, "Couldnt open handle to tID", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (SuspendThread(hThread) == (DWORD)-1)
	{
		CloseHandle(hThread);
		MessageBoxA(0, "SuspendThread failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	CONTEXT oldTC;
	oldTC.ContextFlags = CONTEXT_CONTROL;

	if (!GetThreadContext(hThread, &oldTC))
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		MessageBoxA(0, "GetThreadContext failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	void* allocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!hThread)
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	BYTE Shellcode[] =
	{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                         // - 0x08           -> returned value

			0x48, 0x83, 0xEC, 0x08,                                                 // + 0x00           -> sub rsp, 0x08

			0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00,                               // + 0x04 (+ 0x07)  -> mov [rsp], RipLowPart
			0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00,							// + 0x0B (+ 0x0F)  -> mov [rsp + 0x04], RipHighPart

			0x50, 0x51, 0x52, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53,       // + 0x13           -> push r(a/c/d)x / r(8 - 11)
			0x9C,                                                                   // + 0x1E           -> pushfq

			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // + 0x1F (+ 0x21)  -> mov rax, pRoutine
			0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // + 0x29 (+ 0x2B)  -> mov rcx, pArg

			0x48, 0x83, 0xEC, 0x20,                                                 // + 0x33           -> sub rsp, 0x20
			0xFF, 0xD0,                                                             // + 0x37           -> call rax
			0x48, 0x83, 0xC4, 0x20,                                                 // + 0x39           -> add rsp, 0x20

			0x48, 0x8D, 0x0D, 0xB4, 0xFF, 0xFF, 0xFF,                               // + 0x3D           -> lea rcx, [pCodecave]

			0x48, 0x89, 0x01,                                                       // + 0x44           -> mov [rcx], rax

			0x9D,                                                                   // + 0x47           -> popfq
			0x41, 0x5B, 0x41, 0x5A, 0x41, 0x59, 0x41, 0x58, 0x5A, 0x59, 0x58,       // + 0x48           -> pop r(11-8) / r(d/c/a)x

			0xC6, 0x05, 0xA9, 0xFF, 0xFF, 0xFF, 0x00,								// + 0x53           -> mov byte ptr[$ - 0x57], 0

			0xC3                                                                    // + 0x5A           -> ret
	}; // SIZE = 0x5B (+ 0x08)

	DWORD dwLoRIP = (DWORD)(oldTC.Rip & 0xFFFFFFFF);
	DWORD dwHiRIP = (DWORD)((oldTC.Rip >> 0x20) & 0xFFFFFFFF);

	*(DWORD*)(Shellcode + 15) = dwLoRIP;
	*(DWORD*)(Shellcode + 23) = dwHiRIP;

	*(void**)(Shellcode + 41) = fRoutine;
	*(void**)(Shellcode + 51) = arg;

	oldTC.Rip = (DWORD64)(allocationAddress)+8;

	if (!WriteProcessMemory(hProcess, allocationAddress, Shellcode, sizeof(Shellcode), nullptr))
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "WriteProcessMemory failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (!SetThreadContext(hThread, &oldTC))
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "SetThreadContext failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (ResumeThread(hThread) == (DWORD)-1)
	{
		CloseHandle(hThread);
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "ResumeThread failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	CloseHandle(hThread);

	DWORD timer = GetTickCount();
	BYTE checkByte = 1;

	do
	{
		ReadProcessMemory(hProcess, (void*)((BYTE*)allocationAddress + 6), &checkByte, sizeof(checkByte), nullptr);
		if (GetTickCount() - timer > 5000)
			break;

		Sleep(10);
	} while (checkByte != 0);

	ReadProcessMemory(hProcess, allocationAddress, &remoteAddress, sizeof(remoteAddress), nullptr);

	VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
	return 1;
}

DWORD x64SetWindowHookEx(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress)
{
	void* pAllocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pAllocationAddress)
	{
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	HMODULE hModuleUser32 = (HMODULE)(x64PEBGetModuleHandle(hProcess, L"User32.dll"));

	void* pCallNextHookEx = GetProcAddress(hModuleUser32, "CallNextHookEx");

	BYTE Shellcode[] =
	{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // - 0x18   -> pArg / returned value / rax  ;buffer
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // - 0x10   -> pRoutine                     ;pointer to target function
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // - 0x08   -> CallNextHookEx               ;pointer to CallNextHookEx

			0x55,                                           // + 0x00   -> push rbp                     ;save important registers
			0x54,                                           // + 0x01   -> push rsp
			0x53,                                           // + 0x02   -> push rbx

			0x48, 0x8D, 0x1D, 0xDE, 0xFF, 0xFF, 0xFF,       // + 0x03   -> lea rbx, [pArg]              ;load pointer into rbx

			0x48, 0x83, 0xEC, 0x20,                         // + 0x0A   -> sub rsp, 0x20                ;reserve stack
			0x4D, 0x8B, 0xC8,                               // + 0x0E   -> mov r9,r8                    ;set up arguments for CallNextHookEx
			0x4C, 0x8B, 0xC2,                               // + 0x11   -> mov r8, rdx
			0x48, 0x8B, 0xD1,                               // + 0x14   -> mov rdx,rcx
			0xFF, 0x53, 0x10,                               // + 0x17   -> call [rbx + 0x10]            ;call CallNextHookEx
			0x48, 0x83, 0xC4, 0x20,                         // + 0x1A   -> add rsp, 0x20                ;update stack

			0x48, 0x8B, 0xC8,                               // + 0x1E   -> mov rcx, rax                 ;copy retval into rcx

			0xEB, 0x00,                                     // + 0x21   -> jmp $ + 0x02                 ;jmp to next instruction
			0xC6, 0x05, 0xF8, 0xFF, 0xFF, 0xFF, 0x18,       // + 0x23   -> mov byte ptr[$ - 0x01], 0x1A ;hotpatch jmp above to skip shellcode

			0x48, 0x87, 0x0B,                               // + 0x2A   -> xchg [rbx], rcx              ;store CallNextHookEx retval, load pArg
			0x48, 0x83, 0xEC, 0x20,                         // + 0x2D   -> sub rsp, 0x20                ;reserve stack
			0xFF, 0x53, 0x08,                               // + 0x31   -> call [rbx + 0x08]            ;call pRoutine
			0x48, 0x83, 0xC4, 0x20,                         // + 0x34   -> add rsp, 0x20                ;update stack

			0x48, 0x87, 0x03,                               // + 0x38   -> xchg [rbx], rax              ;store pRoutine retval, restore CallNextHookEx retval

			0x5B,                                          // + 0x3B   -> pop rbx                       ;restore important registers
			0x5C,                                           // + 0x3C   -> pop rsp
			0x5D,                                           // + 0x3D   -> pop rbp

			0xC3                                            // + 0x3E   -> ret                          ;return
	}; // SIZE = 0x3F (+ 0x18)

	*(void**)(Shellcode + 0) = arg;
	*(void**)(Shellcode + 8) = fRoutine;
	*(void**)(Shellcode + 16) = pCallNextHookEx;

	if (!WriteProcessMemory(hProcess, pAllocationAddress, Shellcode, sizeof(Shellcode), nullptr))
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		return 0;
	}

	static EnumWindowsCallbackData data;
	data.m_vecHookData.clear();
	data.m_oHook = (HOOKPROC)((uintptr_t)(pAllocationAddress) + 8);
	data.m_dwPID = GetProcessId(hProcess);
	data.m_hModule = hModuleUser32;

	//stopped at minute 18, learn lambda bitch

	return 1;
}

DWORD x64QueueUserAPC(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress)
{
	void* pAllocationAddress = VirtualAllocEx(hProcess, nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pAllocationAddress)
	{
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	BYTE Shellcode[] =
	{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // - 0x18   -> returned value                           ;buffer to store returned value
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // - 0x10   -> pArg                                     ;buffer to store argument
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // - 0x08   -> pRoutine                                 ;pointer to the rouinte to call

			0xEB, 0x00,                                        // + 0x00   -> jmp $+0x02                              ;jump to the next instruction

			0x48, 0x8B, 0x41, 0x10,                             // + 0x02   -> mov rax, [rcx + 0x10]                    ;move pRoutine into rax
			0x48, 0x8B, 0x49, 0x08,                             // + 0x06   -> mov rcx, [rcx + 0x08]                    ;move pArg into rcx

			0x48, 0x83, 0xEC, 0x28,                             // + 0x0A   -> sub rsp, 0x28                            ;reserve stack
			0xFF, 0xD0,                                         // + 0x0E   -> call rax                                 ;call pRoutine
			0x48, 0x83, 0xC4, 0x28,                             // + 0x10   -> add rsp, 0x28                            ;update stack

			0x48, 0x85, 0xC0,                                   // + 0x14   -> test rax, rax                            ;check if rax indicates success/failure
			0x74, 0x11,                                        // + 0x17   -> je pCodecave + 0x2A                       ;jmp to ret if routine failed

			0x48, 0x8D, 0x0D, 0xC8, 0xFF, 0xFF, 0xFF,           // + 0x19   -> lea rcx, [pCodecave]                     ;load pointer to codecave into rcx
			0x48, 0x89, 0x01,                                   // + 0x20   -> mov [rcx], rax                           ;store returned value

			0xC6, 0x05, 0xD7, 0xFF, 0xFF, 0xFF, 0x28,           // + 0x23   -> mov byte ptr[pCodecave + 0x18], 0x28     ;hot patch jump to skip shellcode

			0xC3                                                // + 0x2A   -> ret                                      ;return
	}; // SIZE = 0x2B (+ 0x10)

	*(void**)(Shellcode + 8) = arg;
	*(void**)(Shellcode + 16) = fRoutine;
	
	if (!WriteProcessMemory(hProcess, pAllocationAddress, Shellcode, sizeof(Shellcode), nullptr))
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "WriteProcessMemory failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	DWORD PID = GetProcessId(hProcess);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, PID);
	if (!hSnapshot)
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	THREADENTRY32 te32{ 0 };
	te32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hSnapshot, &te32))
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		CloseHandle(hSnapshot);
		MessageBoxA(0, "Thread32First failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	bool bAPCQueued = false;
	do
	{
		if (te32.th32OwnerProcessID != PID)
			continue;

		HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, te32.th32ThreadID);
		if (!hThread)
			continue;

		if (QueueUserAPC((PAPCFUNC)((BYTE*)(pAllocationAddress) + 24), hThread, (ULONG_PTR)(pAllocationAddress)))
			bAPCQueued = true;

		CloseHandle(hThread);
	} while(Thread32Next(hSnapshot, &te32));

	CloseHandle(hSnapshot);

	if (!bAPCQueued)
	{
		VirtualFreeEx(hProcess, pAllocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "Thread32First failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	DWORD dwTimer = GetTickCount();
	
	do
	{
		ReadProcessMemory(hProcess, pAllocationAddress, &remoteAddress, sizeof(remoteAddress), nullptr);
		if (GetTickCount() - dwTimer > 5000)
		{
			MessageBoxA(0, "Couldnt get return address of remote address", "ShellCode Injection", MB_OK);
			return 0;
		}
		Sleep(10);
	} while (!remoteAddress);

	return 1;
}

DWORD x64StartRoutine(HANDLE hProcess, LaunchMethod launchMethod, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress)
{
	DWORD dwRet = 0;
	switch (launchMethod)
	{
	case NTCREATETHREAD:
		dwRet = x64NtCreateThreadEx(hProcess, fRoutine, arg, remoteAddress);
		break;
	case HIJACKTHREAD:
		dwRet = x64HijackThread(hProcess, fRoutine, arg, remoteAddress);
		break;
	case SETWINDOWHOOKEX:
		dwRet = x64SetWindowHookEx(hProcess, fRoutine, arg, remoteAddress);
		break;
	case QUEUEUSERAPC:
		dwRet = x64QueueUserAPC(hProcess, fRoutine, arg, remoteAddress);
		break;
	}
	return dwRet;
}

DWORD Inject(const char* processName, const char* dllPath, LaunchMethod launchMethod, InjectMethod injectMethod, DWORD architecture)
{
	if (architecture != x86Process && architecture != x64Process)
		return 0;

	HANDLE hProcess = GetHandleProcessByName(processName, PROCESS_ALL_ACCESS);
	if (!hProcess)
	{
		MessageBoxA(0, "GetHandleProcessByName failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	void* allocationAddress = VirtualAllocEx(hProcess, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!allocationAddress)
	{
		MessageBoxA(0, "VirtualAllocEx failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	if (!WriteProcessMemory(hProcess, allocationAddress, dllPath, strlen(dllPath), nullptr))
	{
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
		MessageBoxA(0, "WriteProcessMemory failed", "ShellCode Injection", MB_OK);
		return 0;
	}

	HMODULE hModuleKernel32;

	if (architecture == x86Process)
	{
		hModuleKernel32 = (HMODULE)x86PEBGetModuleHandle(processName, L"KERNEL32.DLL");
		if (!hModuleKernel32)
		{
			VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
			MessageBoxA(0, "Couldnt get kernel32.dll address", "ShellCode Injection", MB_OK);
			return 0;
		}

		x86tRoutine* fRoutine = nullptr;

		fRoutine = (x86tRoutine*)((DWORD)hModuleKernel32 + 0x30A30);
		if (!fRoutine)
		{
			VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
			MessageBoxA(0, "Couldnt get LoadLibraryA address", "ShellCode Injection", MB_OK);
			return 0;
		}

		DWORD remoteAddress = 0;
		DWORD ret = x86StartRoutine(hProcess, launchMethod, fRoutine, allocationAddress, remoteAddress);
		if (ret == 0)
		{
			VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
			MessageBoxA(0, "StartRoutine failed", "ShellCode Injection", MB_OK);
			return 0;
		}
	}
	else if (architecture == x64Process)
	{
		hModuleKernel32 = (HMODULE)x64PEBGetModuleHandle(processName, L"KERNEL32.DLL");
		if (!hModuleKernel32)
		{
			VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
			MessageBoxA(0, "Couldnt get kernel32.dll address", "ShellCode Injection", MB_OK);
			return 0;
		}

		x64tRoutine* fRoutine = nullptr;

		fRoutine = (x64tRoutine*)GetProcAddress(hModuleKernel32, "LoadLibraryA");
		if (!fRoutine)
		{
			VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
			MessageBoxA(0, "Couldnt get LoadLibraryA address", "ShellCode Injection", MB_OK);
			return 0;
		}

		uintptr_t remoteAddress = 0;
		DWORD ret = x64StartRoutine(hProcess, launchMethod, fRoutine, allocationAddress, remoteAddress);
		if (ret == 0)
		{
			VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
			MessageBoxA(0, "StartRoutine failed", "ShellCode Injection", MB_OK);
			return 0;
		}
	}

	if (launchMethod != QUEUEUSERAPC)
		VirtualFreeEx(hProcess, allocationAddress, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	return true;
}