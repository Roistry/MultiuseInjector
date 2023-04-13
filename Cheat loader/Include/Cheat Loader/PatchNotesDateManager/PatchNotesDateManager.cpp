#include "PatchNotesDateManager.h"

PatchNotesDateManager::PatchNotesDateManager()
{
	selectedDate = 0;
}

void PatchNotesDateManager::DisplaySelectAllDates()
{
	std::vector<const char*> dates;
	for (size_t i = 0; i < vecPatchNotesDates.size(); i++)
		dates.push_back(vecPatchNotesDates[i].date);

	for (size_t i = 0; i < vecPatchNotesDates.size(); i++)
	{
		ImGui::SetCursorPos(ImVec2(0, i * 20));
		ImGui::SelectablesList(dates, ImVec2{ 120, 0 }, ImVec2{ 120, 20 }, &selectedDate, &listOpen);
	}
}

void PatchNotesDateManager::DisplayPatchNoteSelectedDate()
{
	vecPatchNotesDates[selectedDate].DisplayPatchNoteData();
}

bool PatchNotesDateManager::DownloadSelectedDate()
{
	return vecPatchNotesDates[selectedDate].Download();
}