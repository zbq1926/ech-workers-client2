#include "app.h"

// 启动代理进程
void StartProcess()
{
    ServerConfig *cfg = GetCurrentServer();

    char cmdLine[MAX_CMD_LEN];
    char exePath[MAX_PATH];

    // 构建 ech-win.exe 的完整路径
    strcpy(exePath, g_exeDir);
    strcat(exePath, "ech-win.exe");

    if (GetFileAttributes(exePath) == INVALID_FILE_ATTRIBUTES)
    {
        char errMsg[512];
        snprintf(errMsg, sizeof(errMsg), "错误: 找不到 ech-win.exe 文件!\n路径: %s\r\n", exePath);
        AppendLog(errMsg);
        return;
    }

    snprintf(cmdLine, MAX_CMD_LEN, "\"%s\"", exePath);

// 宏定义用于简化参数添加
#define APPEND_ARG(flag, val)   \
    if (strlen(val) > 0)        \
    {                           \
        strcat(cmdLine, " ");   \
        strcat(cmdLine, flag);  \
        strcat(cmdLine, " \""); \
        strcat(cmdLine, val);   \
        strcat(cmdLine, "\"");  \
    }

    APPEND_ARG("-f", cfg->server);
    APPEND_ARG("-l", cfg->listen);
    APPEND_ARG("-token", cfg->token);
    APPEND_ARG("-ip", cfg->ip);

    if (strlen(cfg->dns) > 0 && strcmp(cfg->dns, "dns.alidns.com/dns-query") != 0)
    {
        APPEND_ARG("-dns", cfg->dns);
    }
    if (strlen(cfg->ech) > 0 && strcmp(cfg->ech, "cloudflare-ech.com") != 0)
    {
        APPEND_ARG("-ech", cfg->ech);
    }
    if (strlen(cfg->pyip) > 0)
    {
        APPEND_ARG("-pyip", cfg->pyip);
    }

    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        return;

    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.wShowWindow = SW_HIDE;

    // 设置工作目录为程序所在目录
    if (CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, g_exeDir, &si, &processInfo))
    {
        CloseHandle(hWrite);
        hLogPipe = hRead;
        hLogThread = CreateThread(NULL, 0, LogReaderThread, NULL, 0, NULL);
        isProcessRunning = TRUE;
        EnableWindow(hStartBtn, FALSE);
        EnableWindow(hStopBtn, TRUE);
        EnableWindow(hServerEdit, FALSE);
        EnableWindow(hListenEdit, FALSE);
        EnableWindow(hServerCombo, FALSE);

        char logMsg[512];
        snprintf(logMsg, sizeof(logMsg), "[系统] 已启动服务器: %s\r\n", cfg->name);
        AppendLog(logMsg);
    }
    else
    {
        CloseHandle(hRead);
        CloseHandle(hWrite);

        char errMsg[512];
        snprintf(errMsg, sizeof(errMsg), "[错误] 启动失败，错误代码: %lu\r\n", GetLastError());
        AppendLog(errMsg);
    }
}

// 停止代理进程
void StopProcess()
{
    isProcessRunning = FALSE;

    if (hLogPipe)
    {
        CloseHandle(hLogPipe);
        hLogPipe = NULL;
    }

    if (processInfo.hProcess)
    {
        TerminateProcess(processInfo.hProcess, 0);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        processInfo.hProcess = NULL;
    }

    if (hLogThread)
    {
        if (WaitForSingleObject(hLogThread, 500) == WAIT_TIMEOUT)
        {
            TerminateThread(hLogThread, 0);
        }
        CloseHandle(hLogThread);
        hLogThread = NULL;
    }

    if (IsWindow(hMainWindow))
    {
        EnableWindow(hStartBtn, TRUE);
        EnableWindow(hStopBtn, FALSE);
        EnableWindow(hServerEdit, TRUE);
        EnableWindow(hListenEdit, TRUE);
        EnableWindow(hServerCombo, TRUE);
        AppendLog("[系统] 进程已停止。\r\n");
    }
}

// 日志读取线程
DWORD WINAPI LogReaderThread(LPVOID lpParam)
{
    (void)lpParam;
    char buf[1024];
    char u8Buf[2048];
    DWORD read;

    while (isProcessRunning && hLogPipe)
    {
        if (ReadFile(hLogPipe, buf, sizeof(buf) - 1, &read, NULL) && read > 0)
        {
            buf[read] = 0;
            int wLen = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
            if (wLen > 0)
            {
                WCHAR *wBuf = (WCHAR *)malloc(wLen * sizeof(WCHAR));
                if (wBuf)
                {
                    MultiByteToWideChar(CP_UTF8, 0, buf, -1, wBuf, wLen);
                    WideCharToMultiByte(CP_ACP, 0, wBuf, -1, u8Buf, sizeof(u8Buf), NULL, NULL);
                    AppendLogAsync(u8Buf);
                    free(wBuf);
                }
            }
            else
            {
                AppendLogAsync(buf);
            }
        }
        else
        {
            break;
        }
    }
    return 0;
}