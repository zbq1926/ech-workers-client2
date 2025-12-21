#include "app.h"

// DPI 缩放函数
int Scale(int x) {
    return (x * g_scale) / 100;
}

// 初始化程序所在目录
void InitExeDir() {
    GetModuleFileName(NULL, g_exeDir, MAX_PATH);
    char* lastSlash = strrchr(g_exeDir, '\\');
    if (lastSlash) {
        *(lastSlash + 1) = 0; // 保留最后的反斜杠
    }
}

// 获取配置文件路径
void GetConfigFilePath(char* path, int maxLen) {
    snprintf(path, maxLen, "%sconfig.ini", g_exeDir);
}