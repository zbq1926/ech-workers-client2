#include "app.h"

// 检查是否以管理员权限运行
BOOL IsRunAsAdministrator() {
    BOOL isAdmin = FALSE;
    PSID administratorsGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroup)) {
        CheckTokenMembership(NULL, administratorsGroup, &isAdmin);
        FreeSid(administratorsGroup);
    }
    
    return isAdmin;
}

// 设置开机启动
BOOL SetAutoStart(BOOL enable) {
    HKEY hKey;
    const char* keyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "ECHWorkersClient";
    LONG result;
    
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_WRITE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return FALSE;
    }
    
    if (enable) {
        char exePath[MAX_PATH];
        char cmdLine[MAX_PATH + 20];
        
        GetModuleFileName(NULL, exePath, MAX_PATH);
        snprintf(cmdLine, sizeof(cmdLine), "\"%s\" -autostart", exePath);
        
        result = RegSetValueEx(hKey, valueName, 0, REG_SZ, 
            (BYTE*)cmdLine, (DWORD)(strlen(cmdLine) + 1));
    } else {
        result = RegDeleteValue(hKey, valueName);
    }
    
    RegCloseKey(hKey);
    
    return (result == ERROR_SUCCESS);
}

// 检查是否已设置开机启动
BOOL IsAutoStartEnabled() {
    HKEY hKey;
    const char* keyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "ECHWorkersClient";
    LONG result;
    BOOL enabled = FALSE;
    
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);
    
    if (result == ERROR_SUCCESS) {
        char value[MAX_PATH];
        DWORD valueSize = sizeof(value);
        DWORD type;
        
        result = RegQueryValueEx(hKey, valueName, NULL, &type, (BYTE*)value, &valueSize);
        
        if (result == ERROR_SUCCESS && type == REG_SZ) {
            enabled = TRUE;
        }
        
        RegCloseKey(hKey);
    }
    
    return enabled;
}

// 更新开机启动复选框状态
void UpdateAutoStartCheckbox() {
    if (hAutoStartCheck) {
        BOOL enabled = IsAutoStartEnabled();
        SendMessage(hAutoStartCheck, BM_SETCHECK, enabled ? BST_CHECKED : BST_UNCHECKED, 0);
        g_autoStartEnabled = enabled;
    }
}