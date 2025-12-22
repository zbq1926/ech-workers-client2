#ifndef APP_H
#define APP_H

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Unique name for Mutex
#define SINGLE_INSTANCE_MUTEX_NAME "ECHWorkersClient_Mutex_Unique_ID"

// 图标资源 ID
#define IDI_APP_ICON 101 

// 版本信息
#define APP_VERSION "1.1.1"
#define APP_TITLE "ech win 客户端 v" APP_VERSION

// 缓冲区大小定义
#define MAX_URL_LEN 8192
#define MAX_SMALL_LEN 2048
#define MAX_CMD_LEN 32768
#define MAX_NAME_LEN 256

// 服务器配置限制
#define MAX_SERVERS 50

// 消息定义
#define WM_TRAYICON (WM_USER + 1)
#define WM_APPEND_LOG (WM_USER + 2) 

// 托盘菜单ID
#define ID_TRAY_ICON 9001
#define ID_TRAY_OPEN 9002
#define ID_TRAY_EXIT 9003

// 输入对话框控件ID
#define ID_INPUT_EDIT 2001
#define ID_INPUT_OK 2002
#define ID_INPUT_CANCEL 2003

// 窗口控件ID定义
#define ID_SERVER_COMBO     1000
#define ID_SERVER_ADD       1001
#define ID_SERVER_DELETE    1002
#define ID_SERVER_RENAME    1003
#define ID_SERVER_EDIT      1004
#define ID_LISTEN_EDIT      1005
#define ID_TOKEN_EDIT       1006
#define ID_IP_EDIT          1007
#define ID_DNS_EDIT         1008
#define ID_ECH_EDIT         1009
#define ID_PYIP_EDIT        1010
#define ID_START_BTN        1013
#define ID_STOP_BTN         1014
#define ID_CLEAR_LOG_BTN    1015
#define ID_LOG_EDIT         1016
#define ID_AUTOSTART_CHECK  1018

// 配置结构体
typedef struct {
    char name[MAX_NAME_LEN];
    char dns[MAX_SMALL_LEN];     
    char ech[MAX_SMALL_LEN];     
    char server[MAX_URL_LEN];    
    char ip[MAX_SMALL_LEN];      
    char listen[MAX_SMALL_LEN];  
    char token[MAX_URL_LEN];
    char pyip[MAX_SMALL_LEN];
} ServerConfig;

// 输入对话框数据
typedef struct {
    char* buffer;
    int bufferSize;
    const char* prompt;
    BOOL result;
} InputDialogData;

// 全局变量声明
extern HFONT hFontUI;
extern HFONT hFontLog;
extern HBRUSH hBrushLog;

extern int g_dpi;
extern int g_scale;

extern HWND hMainWindow;
extern HWND hServerCombo;
extern HWND hServerEdit, hListenEdit, hTokenEdit, hIpEdit, hDnsEdit, hEchEdit, hPyipEdit;
extern HWND hStartBtn, hStopBtn, hLogEdit;
extern HWND hAutoStartCheck;
extern PROCESS_INFORMATION processInfo;
extern HANDLE hLogPipe;
extern HANDLE hLogThread;
extern BOOL isProcessRunning;
extern NOTIFYICONDATA nid;
extern BOOL g_isAutoStart;

extern char g_exeDir[MAX_PATH];

extern ServerConfig servers[MAX_SERVERS];
extern int serverCount;
extern int currentServerIndex;
extern BOOL g_autoStartEnabled;

// DPI 缩放函数
int Scale(int x);

// 窗口管理函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK InputDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateControls(HWND hwnd);

// 进程管理函数
void StartProcess();
void StopProcess();
DWORD WINAPI LogReaderThread(LPVOID lpParam);

// 日志函数
void AppendLog(const char* text);
void AppendLogAsync(const char* text);

// 配置函数
void SaveConfig();
void LoadConfig();
void GetControlValues();
void SetControlValues();

// 托盘函数
void InitTrayIcon(HWND hwnd);
void ShowTrayIcon();
void RemoveTrayIcon();

// 路径函数
void InitExeDir();
void GetConfigFilePath(char* path, int maxLen);

// 服务器管理函数
void InitDefaultServer();
void RefreshServerCombo();
void SwitchServer(int index);
void AddNewServer();
void DeleteCurrentServer();
void RenameCurrentServer();
ServerConfig* GetCurrentServer();
BOOL ShowInputDialog(HWND parent, const char* title, const char* prompt, char* buffer, int bufferSize);

// 开机启动函数
BOOL IsRunAsAdministrator();
BOOL SetAutoStart(BOOL enable);
BOOL IsAutoStartEnabled();
void UpdateAutoStartCheckbox();

#endif // APP_H