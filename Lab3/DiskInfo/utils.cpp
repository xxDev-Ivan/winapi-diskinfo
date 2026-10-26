#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "utils.h"

const char* G_LOG_FILE_PATH = "DiskInfo_log.txt";

void WriteToLog(const char* eventType, const char* fileName)
{
    FILE* f = fopen(G_LOG_FILE_PATH, "a");
    if (f == NULL) {
        printf("  ПОМИЛКА ЛОГУВАННЯ: не вдалося відкрити %s\n", G_LOG_FILE_PATH);
        return;
    }
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now);
    char tb[64];
    strftime(tb, sizeof(tb), "%Y-%m-%d %H:%M:%S", &t);
    fprintf(f, "[%s] | %s | %s\n", tb, eventType, fileName ? fileName : "-");
    fclose(f);
}