#include "PatchNotesDate.h"

PatchNotesDate::PatchNotesDate(const char* date, const char* downloadURL, const char* patchNoteData)
{
    this->date = date;
    this->downloadURL = downloadURL;
    this->patchNoteData = patchNoteData;
}

void PatchNotesDate::DisplayPatchNoteData()
{
    ImGui::Text(patchNoteData);
}

bool PatchNotesDate::Download()
{
    char* newStr1 = new char[strlen(date)]; //newStr1 not getting deleted causing a small memory leak
    strcpy_s(newStr1, strlen(date) + 1, date);
    *strchr(newStr1, '/') = '-';
    *strchr(newStr1, '/') = '-';

    char* newStr2 = combineChars(newStr1, (char*)" Cheat loader.exe");

    if (S_OK != URLDownloadToFileA(0, downloadURL, newStr2, 0, 0))
    {
        delete[] newStr2;
        return false;
    }
    delete[] newStr2;
    return true;
}
