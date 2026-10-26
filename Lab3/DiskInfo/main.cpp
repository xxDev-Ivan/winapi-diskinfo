#include <windows.h>
#include <stdio.h>
#include "DiskInfo.h"
#include "MemoryInfo.h"
#include "UserInfo.h"
#include "Watcher.h"

int main()
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    SetErrorMode(SEM_FAILCRITICALERRORS);

    DiskInfo();        // 1-4
    MemoryInfo();      // 5
    ComputerInfo();    // 6
    UserInfo();        // 7
    Directories();     // 8
    WatchDirectory();  // 9

    printf("\nНатисни Enter для виходу...");
    getchar();
    return 0;
}