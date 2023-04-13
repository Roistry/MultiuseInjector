#pragma once
#include <urlmon.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "Cheat Loader/InjectorUtils/InjectorUtils.h"

#pragma comment(lib, "Urlmon.lib")

class PatchNotesDate
{
public:
	const char* date;
	const char* downloadURL;
	const char* patchNoteData;

	PatchNotesDate(const char* date, const char* downloadURL, const char* patchNoteData);
	void DisplayPatchNoteData();
	bool Download();
};