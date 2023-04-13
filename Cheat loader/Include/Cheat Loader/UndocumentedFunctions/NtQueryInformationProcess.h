#pragma once
#include <Windows.h>

#include "../UndocumentedStructs/PROCESSINFOCLASS.h"

typedef NTSTATUS(NTAPI* tNtQueryInformationProcess)(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _Out_ PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength,
    _Out_opt_ PULONG ReturnLength
    );