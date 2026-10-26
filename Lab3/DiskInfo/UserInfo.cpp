#include <windows.h>
#include <lmcons.h>
#include <stdio.h>
#include "UserInfo.h"

// п.6
void ComputerInfo()
{
    printf("\n## 6. Ім'я комп'ютера\n");

    char name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(name);
    if (GetComputerNameA(name, &size))
        printf("  GetComputerName: %s\n", name);
    else
        printf("  Помилка GetComputerName: %lu\n", GetLastError());

    // розширена версія: GetComputerNameEx
    COMPUTER_NAME_FORMAT fmts[] = {
        ComputerNameNetBIOS, ComputerNameDnsHostname, ComputerNameDnsDomain,
        ComputerNameDnsFullyQualified, ComputerNamePhysicalNetBIOS,
        ComputerNamePhysicalDnsHostname, ComputerNamePhysicalDnsFullyQualified
    };
    const char* titles[] = {
        "NetBIOS", "DNS hostname", "DNS domain", "DNS FQDN (повне)",
        "Physical NetBIOS", "Physical DNS hostname", "Physical DNS FQDN"
    };
    for (int i = 0; i < 7; i++) {
        char buf[256];
        DWORD sz = sizeof(buf);
        if (GetComputerNameExA(fmts[i], buf, &sz))
            printf("  %-22s: %s\n", titles[i], buf[0] ? buf : "(порожньо)");
    }
}

// п.7
void UserInfo()
{
    printf("\n## 7. Поточний користувач\n");

    char user[UNLEN + 1];
    DWORD size = UNLEN + 1;
    if (GetUserNameA(user, &size))
        printf("  Ім'я користувача: %s\n", user);
    else
        printf("  Помилка GetUserName: %lu\n", GetLastError());

    // опційно: чи є права адміністратора (UAC)
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION el;
        DWORD ret = 0;
        if (GetTokenInformation(token, TokenElevation, &el, sizeof(el), &ret))
            printf("  Запущено з правами адміністратора: %s\n", el.TokenIsElevated ? "так" : "ні");
        CloseHandle(token);
    }
}

// п.8
void Directories()
{
    printf("\n## 8. Системні каталоги\n");
    char p[MAX_PATH];

    if (GetSystemDirectoryA(p, MAX_PATH))
        printf("  Системний каталог (System32): %s\n", p);
    else
        printf("  Помилка GetSystemDirectory: %lu\n", GetLastError());

    if (GetWindowsDirectoryA(p, MAX_PATH))
        printf("  Каталог Windows:              %s\n", p);
    else
        printf("  Помилка GetWindowsDirectory: %lu\n", GetLastError());

    if (GetTempPathA(MAX_PATH, p))
        printf("  Тимчасовий каталог:           %s\n", p);
    else
        printf("  Помилка GetTempPath: %lu\n", GetLastError());

    if (GetCurrentDirectoryA(MAX_PATH, p))
        printf("  Робочий каталог програми:     %s\n", p);
    else
        printf("  Помилка GetCurrentDirectory: %lu\n", GetLastError());
}