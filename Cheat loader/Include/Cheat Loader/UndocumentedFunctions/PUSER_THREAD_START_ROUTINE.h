#pragma once
#include <Windows.h>

typedef NTSTATUS(NTAPI* PUSER_THREAD_START_ROUTINE)(
        _In_ PVOID ThreadParameter
        );