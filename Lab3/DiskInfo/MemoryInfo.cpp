#include <windows.h>
#include <stdio.h>
#include "MemoryInfo.h"

void MemoryInfo()
{
    printf("\n## 5. Системна пам'ять\n");
    const double MB = 1024.0 * 1024.0;

    // GlobalMemoryStatus (старий)
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatus(&ms);

    printf("[GlobalMemoryStatus]\n");
    printf("  Завантаження RAM: %lu %%\n", ms.dwMemoryLoad);
    printf("  Фізична пам'ять (всього): %.2f МБ\n", ms.dwTotalPhys / MB);
    printf("  Фізична пам'ять (вільно): %.2f МБ\n", ms.dwAvailPhys / MB);
    printf("  Pagefile (всього): %.2f МБ\n", ms.dwTotalPageFile / MB);
    printf("  Pagefile (вільно): %.2f МБ\n", ms.dwAvailPageFile / MB);
    printf("  Віртуальна пам'ять (всього): %.2f МБ\n", ms.dwTotalVirtual / MB);
    printf("  Віртуальна пам'ять (вільно): %.2f МБ\n", ms.dwAvailVirtual / MB);

    // GlobalMemoryStatusEx (розширений, коректно для >4 ГБ)
    MEMORYSTATUSEX mx;
    mx.dwLength = sizeof(mx);
    if (GlobalMemoryStatusEx(&mx)) {
        printf("[GlobalMemoryStatusEx]\n");
        printf("  Завантаження RAM: %lu %%\n", mx.dwMemoryLoad);
        printf("  Фізична пам'ять: всього %.2f МБ, вільно %.2f МБ\n", mx.ullTotalPhys / MB, mx.ullAvailPhys / MB);
        printf("  Pagefile: всього %.2f МБ, вільно %.2f МБ\n", mx.ullTotalPageFile / MB, mx.ullAvailPageFile / MB);
        printf("  Віртуальна пам'ять: всього %.2f МБ, вільно %.2f МБ\n", mx.ullTotalVirtual / MB, mx.ullAvailVirtual / MB);
    }
    else {
        printf("  Помилка GlobalMemoryStatusEx: %lu\n", GetLastError());
    }
}