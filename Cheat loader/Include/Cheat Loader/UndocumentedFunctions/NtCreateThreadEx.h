#pragma once
#include <Windows.h>

#include "../UndocumentedStructs/OBJECT_ATTRIBUTES.h"
#include "PUSER_THREAD_START_ROUTINE.h"
#include "../UndocumentedStructs/PS_ATTRIBUTE_LIST.h"

typedef NTSTATUS(NTAPI* tNtCreateThreadEx)(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _In_ PUSER_THREAD_START_ROUTINE StartRoutine,
    _In_opt_ PVOID Argument,
    _In_ ULONG CreateFlags,
    _In_opt_ ULONG_PTR ZeroBits,
    _In_opt_ SIZE_T StackSize,
    _In_opt_ SIZE_T MaximumStackSize,
    _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
    );