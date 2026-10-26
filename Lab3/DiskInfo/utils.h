#pragma once

extern const char* G_LOG_FILE_PATH;

// Запис у лог: час, тип події, ім'я файлу
void WriteToLog(const char* eventType, const char* fileName);