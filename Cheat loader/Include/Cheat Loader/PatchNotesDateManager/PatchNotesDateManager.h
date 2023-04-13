#pragma once
#include <vector>

#include "Cheat Loader/InjectorUtils/InjectorUtils.h"
#include "Cheat Loader/PatchNotesDate/PatchNotesDate.h"
#include "ImGui/imimgui.h"

class PatchNotesDateManager
{
public:
    std::vector<PatchNotesDate> vecPatchNotesDates;
    int selectedDate;
    bool listOpen;

    PatchNotesDateManager();
    void DisplaySelectAllDates();
    void DisplayPatchNoteSelectedDate();
    bool DownloadSelectedDate();
};