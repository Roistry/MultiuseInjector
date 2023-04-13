#pragma once
#include <Windows.h>

#include "UNICODE_STRING.h"

typedef struct _CURDIR
{
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, * PCURDIR;