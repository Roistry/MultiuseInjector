#pragma once
#include <fstream>

#include <Windows.h>
#include <TlHelp32.h>

#include "Cheat Loader/UndocumentedFunctions/NtCreateThreadEx.h"
#include "Cheat Loader/UndocumentedFunctions/NtQueryInformationProcess.h"

#include "Cheat Loader/UndocumentedStructs/PEB.h"
#include "Cheat Loader/UndocumentedStructs/PROCESS_BASIC_INFORMATION.h"
#include "Cheat Loader/UndocumentedStructs/PROCESSINFOCLASS.h"
#include "Cheat Loader/UndocumentedStructs/LDR_DATA_TABLE_ENTRY.h"

#define x86Process 1
#define x64Process 2

typedef uintptr_t(__fastcall* x64tRoutine)(void* arg);

typedef uintptr_t(__stdcall* x86tRoutine)(void* arg);

enum InjectMethod
{
	LOADLIBRARYA,
	MANUALMAP
};

enum LaunchMethod
{
	NTCREATETHREAD,
	HIJACKTHREAD,
	SETWINDOWHOOKEX,
	QUEUEUSERAPC
};

static tNtQueryInformationProcess oNtQueryInformationProcess = 0;

static tNtCreateThreadEx oNtCreateThreadEx = 0;

DWORD GetPID(char* processName);

char* GetProcessName(DWORD pID);

DWORD IsWow64(DWORD pID);

bool ValidFile(char* fileName);

bool RemoveFile(char* fileName);

char* combineChars(char* str1, char* str2);

DWORD x86GetPEB(HANDLE hProcess);

PEB x64GetPEB(HANDLE hProcess);

DWORD GetProcessIDByName(const char* processName);

HANDLE GetHandleProcessByName(const char* processName, DWORD dwDesiredAccess);

void* x64PEBGetModuleHandle(const char* processName, const wchar_t* moduleName);

void* x86PEBGetModuleHandle(const char* processName, const wchar_t* moduleName);

DWORD x86NtCreateThreadEx(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress);

DWORD x86HijackThread(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress);

DWORD x86SetWindowHookEx(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress);

DWORD x86QueueUserAPC(HANDLE hProcess, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress);

DWORD x86StartRoutine(HANDLE hProcess, LaunchMethod launchMethod, x86tRoutine* fRoutine, void* arg, DWORD& remoteAddress);

DWORD x64NtCreateThreadEx(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress);

DWORD x64HijackThread(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress);

DWORD x64SetWindowHookEx(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress);

DWORD x64QueueUserAPC(HANDLE hProcess, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress);

DWORD x64StartRoutine(HANDLE hProcess, LaunchMethod launchMethod, x64tRoutine* fRoutine, void* arg, uintptr_t& remoteAddress);

DWORD Inject(const char* processName, const char* dllPath, LaunchMethod launchMethod, InjectMethod injectMethod, DWORD architecture);