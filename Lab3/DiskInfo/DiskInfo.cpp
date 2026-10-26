#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "DiskInfo.h"

static const char* DriveTypeText(UINT t)
{
    switch (t) {
    case DRIVE_FIXED:       return "DRIVE_FIXED - HDD/SSD (стаціонарний накопичувач; системний диск, сховище даних)";
    case DRIVE_REMOVABLE:   return "DRIVE_REMOVABLE - USB Flash / SD (змінний носій; перенесення файлів)";
    case DRIVE_CDROM:       return "DRIVE_CDROM - CD/DVD привід (оптичні диски; інсталяційні образи)";
    case DRIVE_REMOTE:      return "DRIVE_REMOTE - мережевий диск (спільна папка на іншому ПК/сервері)";
    case DRIVE_RAMDISK:     return "DRIVE_RAMDISK - RAM-диск (диск в оперативній пам'яті; дуже швидкий, тимчасовий)";
    case DRIVE_NO_ROOT_DIR: return "DRIVE_NO_ROOT_DIR - кореневий каталог відсутній";
    default:                return "DRIVE_UNKNOWN - неможливо визначити тип";
    }
}

static void PrintFlag(DWORD flags, DWORD bit, const char* name)
{
    printf("      [%c] %s\n", (flags & bit) ? '+' : '-', name);
}

void DiskInfo()
{
    printf("\n## 1-4. Інформація про диски\n");

    // п.1: GetLogicalDrives (бітова маска)
    DWORD mask = GetLogicalDrives();
    if (mask == 0) {
        printf("Помилка GetLogicalDrives: %lu\n", GetLastError());
        return;
    }
    printf("Бітова маска GetLogicalDrives: 0x%08lX\n", mask);

    // п.1: GetLogicalDriveStrings (повний перелік)
    char drives[512];
    DWORD len = GetLogicalDriveStringsA(sizeof(drives), drives);
    if (len == 0 || len > sizeof(drives)) {
        printf("Помилка GetLogicalDriveStrings: %lu\n", GetLastError());
        return;
    }

    for (char* d = drives; *d; d += strlen(d) + 1)
    {
        printf("\n------------------------------------------\n");
        printf("Буква диска: %c\n", d[0]);
        printf("Повний шлях: %s\n", d);

        // п.2: тип
        UINT type = GetDriveTypeA(d);
        printf("Тип: %s\n", DriveTypeText(type));

        // п.3: файлова система
        char label[MAX_PATH + 1] = "";
        char fsName[MAX_PATH + 1] = "";
        DWORD serial = 0, maxLen = 0, flags = 0;

        if (GetVolumeInformationA(d, label, sizeof(label), &serial, &maxLen,
            &flags, fsName, sizeof(fsName)))
        {
            printf("Статус: ДОСТУПНИЙ\n");
            printf("  Мітка тому: %s\n", label[0] ? label : "(немає)");
            printf("  Серійний номер: %04X-%04X\n", (serial >> 16) & 0xFFFF, serial & 0xFFFF);
            printf("  Макс. довжина імені файлу: %lu\n", maxLen);
            printf("  Файлова система: %s\n", fsName);
            printf("  Прапори файлової системи:\n");
            PrintFlag(flags, FILE_FILE_COMPRESSION, "стиснення файлів");
            PrintFlag(flags, FILE_VOLUME_IS_COMPRESSED, "том стиснений");
            PrintFlag(flags, FILE_SUPPORTS_ENCRYPTION, "шифрування (EFS)");
            PrintFlag(flags, FILE_PERSISTENT_ACLS, "ACL (списки прав доступу)");
            PrintFlag(flags, FILE_SUPPORTS_USN_JOURNAL, "журнал USN (NTFS)");
            PrintFlag(flags, FILE_CASE_SENSITIVE_SEARCH, "пошук з урахуванням регістру");
            PrintFlag(flags, FILE_CASE_PRESERVED_NAMES, "збереження регістру імен");
            PrintFlag(flags, FILE_UNICODE_ON_DISK, "Unicode імена файлів");
            PrintFlag(flags, FILE_SUPPORTS_TRANSACTIONS, "транзакції");
            PrintFlag(flags, FILE_SUPPORTS_SPARSE_FILES, "розріджені файли");
            PrintFlag(flags, FILE_SUPPORTS_REPARSE_POINTS, "точки повторного аналізу");
            PrintFlag(flags, FILE_READ_ONLY_VOLUME, "том лише для читання");
        }
        else {
            printf("Статус: НЕДОСТУПНИЙ (код помилки %lu, напр. немає носія в приводі)\n", GetLastError());
            continue;
        }

        // п.4: місце
        ULARGE_INTEGER avail, total, freeBytes;
        if (GetDiskFreeSpaceExA(d, &avail, &total, &freeBytes))
        {
            const double GB = 1024.0 * 1024.0 * 1024.0;
            const double MB = 1024.0 * 1024.0;
            double used = (double)(total.QuadPart - freeBytes.QuadPart);

            printf("  Загальний розмір: %.2f ГБ (%.0f МБ)\n", total.QuadPart / GB, total.QuadPart / MB);
            printf("  Вільне місце: %.2f ГБ (%.0f МБ)\n", freeBytes.QuadPart / GB, freeBytes.QuadPart / MB);
            printf("  Доступний простір (для користувача): %.2f ГБ\n", avail.QuadPart / GB);
            if (total.QuadPart > 0)
                printf("  Завантаження диска: %.1f %%\n", used * 100.0 / (double)total.QuadPart);
        }
        else {
            printf("  Помилка GetDiskFreeSpaceEx: %lu\n", GetLastError());
        }
    }
}