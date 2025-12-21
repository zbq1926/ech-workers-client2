#include "app.h"

// 全局变量定义
HFONT hFontUI = NULL;
HFONT hFontLog = NULL;
HBRUSH hBrushLog = NULL;

int g_dpi = 96;
int g_scale = 100;

HWND hMainWindow;
HWND hServerCombo;
HWND hServerEdit, hListenEdit, hTokenEdit, hIpEdit, hDnsEdit, hEchEdit, hPyipEdit;
HWND hStartBtn, hStopBtn, hLogEdit;
HWND hAutoStartCheck;
PROCESS_INFORMATION processInfo;
HANDLE hLogPipe = NULL;
HANDLE hLogThread = NULL;
BOOL isProcessRunning = FALSE;
NOTIFYICONDATA nid;
BOOL g_isAutoStart = FALSE;

char g_exeDir[MAX_PATH] = {0};

ServerConfig servers[MAX_SERVERS];
int serverCount = 0;
int currentServerIndex = 0;
BOOL g_autoStartEnabled = FALSE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    
    // 初始化程序所在目录
    InitExeDir();
    
    // 解析命令行参数
    if (lpCmdLine && strstr(lpCmdLine, "-autostart")) {
        g_isAutoStart = TRUE;
    }
    
    // --- 单实例检查开始 ---
    HANDLE hMutex = CreateMutex(NULL, TRUE, SINGLE_INSTANCE_MUTEX_NAME);

    if (hMutex != NULL && GetLastError() == ERROR_ALREADY_EXISTS) {
        HWND hExistingWnd = FindWindow("ECHWorkersClient", NULL); 
        if (hExistingWnd) {
            PostMessage(hExistingWnd, WM_TRAYICON, ID_TRAY_ICON, WM_LBUTTONUP);
        }
        CloseHandle(hMutex);
        return 0; 
    }
    
    // 设置 DPI 感知
    HMODULE hUser32 = LoadLibrary("user32.dll");
    if (hUser32) {
        typedef BOOL (WINAPI *SetProcessDPIAwareFunc)(void);
        SetProcessDPIAwareFunc setDPIAware = (SetProcessDPIAwareFunc)(void*)GetProcAddress(hUser32, "SetProcessDPIAware");
        if (setDPIAware) setDPIAware();
        FreeLibrary(hUser32);
    }
    
    HDC hdc = GetDC(NULL);
    g_dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    g_scale = (g_dpi * 100) / 96;
    ReleaseDC(NULL, hdc);
    
    // 初始化公共控件
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // 创建字体
    hFontUI = CreateFont(Scale(19), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Microsoft YaHei UI");

    hFontLog = CreateFont(Scale(16), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");

    hBrushLog = CreateSolidBrush(RGB(255, 255, 255));

    // 注册主窗口类
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ECHWorkersClient";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    if (!wc.hIcon) wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClass(&wc)) return 1;

    // 注册输入对话框窗口类
    WNDCLASS wcInput = {0};
    wcInput.lpfnWndProc = InputDialogProc;
    wcInput.hInstance = hInstance;
    wcInput.lpszClassName = "InputDialog";
    wcInput.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcInput.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcInput.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClass(&wcInput);

    // 创建主窗口
    int winWidth = Scale(900);
    int winHeight = Scale(800);
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN;

    hMainWindow = CreateWindowEx(
        0, "ECHWorkersClient", APP_TITLE, 
        winStyle,
        (screenW - winWidth) / 2, (screenH - winHeight) / 2, 
        winWidth, winHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!hMainWindow) return 1;

    // 初始化托盘图标
    InitTrayIcon(hMainWindow);
    ShowTrayIcon();

    // 如果是开机自动启动，启动时隐藏窗口
    if (g_isAutoStart) {
        ShowWindow(hMainWindow, SW_HIDE);
    } else {
        ShowWindow(hMainWindow, nCmdShow);
    }
    
    UpdateWindow(hMainWindow);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_TAB) {
            IsDialogMessage(hMainWindow, &msg);
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    CloseHandle(hMutex); 

    return (int)msg.wParam;
}