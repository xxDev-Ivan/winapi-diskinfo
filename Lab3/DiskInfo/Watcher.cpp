#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include "Watcher.h"
#include "utils.h"

// FindFirstChangeNotification не каже ім'я файлу, тому порівнюємо "знімки" каталогу
struct FileInfo {
    FILETIME lastWrite;
    ULONGLONG size;
    DWORD attrs;
};
typedef std::map<std::string, FileInfo> Snapshot;

static Snapshot TakeSnapshot(const std::string& dir)
{
    Snapshot s;
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((dir + "\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return s;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;
        FileInfo fi;
        fi.lastWrite = fd.ftLastWriteTime;
        fi.size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
        fi.attrs = fd.dwFileAttributes;
        s[fd.cFileName] = fi;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    return s;
}

static void Report(const char* type, const char* name)
{
    printf("  [%s] %s\n", type, name);
    WriteToLog(type, name);
}

static void Compare(const Snapshot& oldS, const Snapshot& newS)
{
    for (const auto& e : newS) {
        auto it = oldS.find(e.first);
        if (it == oldS.end()) {
            Report((e.second.attrs & FILE_ATTRIBUTE_DIRECTORY) ? "СТВОРЕНО ПАПКУ (DIR_NAME)"
                : "СТВОРЕНО ФАЙЛ (FILE_NAME)", e.first.c_str());
            continue;
        }
        const FileInfo& o = it->second;
        const FileInfo& n = e.second;
        if (o.size != n.size)
            Report("ЗМІНА РОЗМІРУ (SIZE)", e.first.c_str());
        if (CompareFileTime(&o.lastWrite, &n.lastWrite) != 0)
            Report("МОДИФІКАЦІЯ (LAST_WRITE)", e.first.c_str());
        if (o.attrs != n.attrs)
            Report("ЗМІНА АТРИБУТІВ (ATTRIBUTES)", e.first.c_str());
    }
    for (const auto& e : oldS) {
        if (newS.find(e.first) == newS.end())
            Report("ВИДАЛЕНО (FILE_NAME/DIR_NAME)", e.first.c_str());
    }
}

void WatchDirectory()
{
    printf("\n## 9. Спостереження за змінами в каталозі\n");

    std::string path = "C:\\DiskInfoWatch";   // за потреби зміни, напр. "D:\\TestForTask9"
    if (!CreateDirectoryA(path.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        printf("  Не вдалося створити %s (код %lu). Зміни шлях у Watcher.cpp.\n", path.c_str(), GetLastError());
        return;
    }
    printf("  Спостерігаю за: %s\n  Лог-файл: %s\n", path.c_str(), G_LOG_FILE_PATH);
    WriteToLog("СТАРТ СПОСТЕРЕЖЕННЯ", path.c_str());

    HANDLE h = FindFirstChangeNotificationA(
        path.c_str(),
        FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME |
        FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_ATTRIBUTES |
        FILE_NOTIFY_CHANGE_SIZE);

    if (h == INVALID_HANDLE_VALUE) {
        printf("  Помилка FindFirstChangeNotification: %lu\n", GetLastError());
        WriteToLog("ПОМИЛКА FindFirstChangeNotification", "-");
        return;
    }

    Snapshot oldS = TakeSnapshot(path);
    printf("  Зроби зміни у цій папці (створи/зміни/перейменуй/видали файл). Очікую 5 подій...\n");

    for (int i = 0; i < 5; i++) {
        printf("\n  Очікування події %d/5...\n", i + 1);
        DWORD st = WaitForSingleObject(h, INFINITE);
        if (st != WAIT_OBJECT_0) {
            printf("  Помилка WaitForSingleObject: %lu\n", GetLastError());
            break;
        }
        Sleep(300); // дати системі завершити запис
        Snapshot newS = TakeSnapshot(path);
        Compare(oldS, newS);
        oldS = newS;

        if (!FindNextChangeNotification(h)) {
            printf("  Помилка FindNextChangeNotification: %lu\n", GetLastError());
            break;
        }
    }

    WriteToLog("СТОП СПОСТЕРЕЖЕННЯ", path.c_str());
    FindCloseChangeNotification(h);
    printf("\n  Спостереження завершено.\n");
}