#include "app.h"

// 创建标签和编辑框控件
void CreateLabelAndEdit(HWND parent, const char* labelText, int x, int y, int w, int h, int editId, HWND* outEdit, BOOL numberOnly) {
    HWND hStatic = CreateWindow("STATIC", labelText, WS_VISIBLE | WS_CHILD | SS_LEFT, 
        x, y + Scale(3), Scale(100), Scale(20), parent, NULL, NULL, NULL);
    SendMessage(hStatic, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    DWORD style = WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL;
    if (numberOnly) style |= ES_NUMBER | ES_CENTER;

    *outEdit = CreateWindow("EDIT", "", style, 
        x + Scale(110), y, w - Scale(110), h, parent, (HMENU)(intptr_t)editId, NULL, NULL);
    SendMessage(*outEdit, WM_SETFONT, (WPARAM)hFontUI, TRUE);
    SendMessage(*outEdit, EM_SETLIMITTEXT, (editId == ID_SERVER_EDIT || editId == ID_TOKEN_EDIT) ? MAX_URL_LEN : MAX_SMALL_LEN, 0);
}

// 创建所有控件
void CreateControls(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    int winW = rect.right;
    int margin = Scale(20);
    int groupW = winW - (margin * 2);
    int lineHeight = Scale(30);
    int lineGap = Scale(10);
    int editH = Scale(26);
    int curY = margin;

    // ========== 服务器管理区域 ==========
    int serverMgrH = Scale(70);
    HWND hGroupServer = CreateWindow("BUTTON", "服务器管理", WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
        margin, curY, groupW, serverMgrH, hwnd, NULL, NULL, NULL);
    SendMessage(hGroupServer, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    int innerY = curY + Scale(25);
    int innerX = margin + Scale(15);

    HWND hLblServer = CreateWindow("STATIC", "选择服务器:", WS_VISIBLE | WS_CHILD, 
        innerX, innerY + Scale(3), Scale(100), Scale(20), hwnd, NULL, NULL, NULL);
    SendMessage(hLblServer, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    hServerCombo = CreateWindow("COMBOBOX", "", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
        innerX + Scale(110), innerY, Scale(350), Scale(200), 
        hwnd, (HMENU)ID_SERVER_COMBO, NULL, NULL);
    SendMessage(hServerCombo, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    int btnX = innerX + Scale(110) + Scale(360);
    int btnW = Scale(70);
    int btnH = Scale(26);
    int btnGap = Scale(8);

    HWND hBtnAdd = CreateWindow("BUTTON", "新增", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        btnX, innerY, btnW, btnH, hwnd, (HMENU)ID_SERVER_ADD, NULL, NULL);
    SendMessage(hBtnAdd, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    HWND hBtnRename = CreateWindow("BUTTON", "重命名", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        btnX + btnW + btnGap, innerY, btnW, btnH, hwnd, (HMENU)ID_SERVER_RENAME, NULL, NULL);
    SendMessage(hBtnRename, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    HWND hBtnDelete = CreateWindow("BUTTON", "删除", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        btnX + (btnW + btnGap) * 2, innerY, btnW, btnH, hwnd, (HMENU)ID_SERVER_DELETE, NULL, NULL);
    SendMessage(hBtnDelete, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    curY += serverMgrH + Scale(15);

    // ========== 核心配置 ==========
    int group1H = Scale(110);
    HWND hGroup1 = CreateWindow("BUTTON", "核心配置", WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
        margin, curY, groupW, group1H, hwnd, NULL, NULL, NULL);
    SendMessage(hGroup1, WM_SETFONT, (WPARAM)hFontUI, TRUE);
    
    innerY = curY + Scale(25);
    
    int midGap = Scale(20); 
    int halfW = (groupW - Scale(30) - midGap) / 2; 

    CreateLabelAndEdit(hwnd, "服务地址:", margin + Scale(15), innerY, groupW - Scale(30), editH, ID_SERVER_EDIT, &hServerEdit, FALSE);
    innerY += lineHeight + lineGap;

    CreateLabelAndEdit(hwnd, "监听地址:", margin + Scale(15), innerY, groupW - Scale(30), editH, ID_LISTEN_EDIT, &hListenEdit, FALSE);

    curY += group1H + Scale(15);

    // ========== 高级选项 ==========
    int group2H = Scale(180);  // 增加高度以适应新布局
    HWND hGroup2 = CreateWindow("BUTTON", "高级选项 (可选)", WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
        margin, curY, groupW, group2H, hwnd, NULL, NULL, NULL);
    SendMessage(hGroup2, WM_SETFONT, (WPARAM)hFontUI, TRUE);
    innerY = curY + Scale(25);

    int col2X = margin + Scale(15) + halfW + midGap;

    CreateLabelAndEdit(hwnd, "TOKEN:", margin + Scale(15), innerY, groupW - Scale(30), editH, ID_TOKEN_EDIT, &hTokenEdit, FALSE);
    innerY += lineHeight + lineGap;

    CreateLabelAndEdit(hwnd, "优选IP(域名):", margin + Scale(15), innerY, halfW, editH, ID_IP_EDIT, &hIpEdit, FALSE);
    CreateLabelAndEdit(hwnd, "反代IP(域名):", col2X, innerY, halfW, editH, ID_PYIP_EDIT, &hPyipEdit, FALSE);
    innerY += lineHeight + lineGap;

    CreateLabelAndEdit(hwnd, "ECH域名:", margin + Scale(15), innerY, halfW, editH, ID_ECH_EDIT, &hEchEdit, FALSE);
    CreateLabelAndEdit(hwnd, "DOH服务器:", col2X, innerY, halfW, editH, ID_DNS_EDIT, &hDnsEdit, FALSE);

    curY += group2H + Scale(15);

    // ========== 按钮栏 ==========
    int btnW2 = Scale(120);
    int btnH2 = Scale(38);
    int btnGap2 = Scale(20);
    int startX = margin;

    hStartBtn = CreateWindow("BUTTON", "启动代理", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        startX, curY, btnW2, btnH2, hwnd, (HMENU)ID_START_BTN, NULL, NULL);
    SendMessage(hStartBtn, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    hStopBtn = CreateWindow("BUTTON", "停止", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        startX + btnW2 + btnGap2, curY, btnW2, btnH2, hwnd, (HMENU)ID_STOP_BTN, NULL, NULL);
    SendMessage(hStopBtn, WM_SETFONT, (WPARAM)hFontUI, TRUE);
    EnableWindow(hStopBtn, FALSE);

    // 开机启动复选框
    hAutoStartCheck = CreateWindow("BUTTON", "开机启动", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_AUTOCHECKBOX,
        startX + btnW2 * 2 + btnGap2 * 2, curY + Scale(10), Scale(120), Scale(22), 
        hwnd, (HMENU)ID_AUTOSTART_CHECK, NULL, NULL);
    SendMessage(hAutoStartCheck, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    HWND hClrBtn = CreateWindow("BUTTON", "清空日志", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        rect.right - margin - btnW2, curY, btnW2, btnH2, hwnd, (HMENU)ID_CLEAR_LOG_BTN, NULL, NULL);
    SendMessage(hClrBtn, WM_SETFONT, (WPARAM)hFontUI, TRUE);

    curY += btnH2 + Scale(15);

    // ========== 日志区域 ==========
    HWND hLogLabel = CreateWindow("STATIC", "运行日志:", WS_VISIBLE | WS_CHILD, 
        margin, curY, Scale(100), Scale(20), hwnd, NULL, NULL, NULL);
    SendMessage(hLogLabel, WM_SETFONT, (WPARAM)hFontUI, TRUE);
    
    curY += Scale(25);

    hLogEdit = CreateWindow("EDIT", "", 
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY, 
        margin, curY, winW - (margin * 2), Scale(200), hwnd, (HMENU)ID_LOG_EDIT, NULL, NULL);
    SendMessage(hLogEdit, WM_SETFONT, (WPARAM)hFontLog, TRUE);
    SendMessage(hLogEdit, EM_SETLIMITTEXT, 0, 0);
}

// 窗口消息处理函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateControls(hwnd);
            LoadConfig();
            if (serverCount == 0) InitDefaultServer();
            RefreshServerCombo();
            SetControlValues();
            UpdateAutoStartCheckbox();
            
            // 如果是开机自动启动，延迟1秒后自动开始连接
            if (g_isAutoStart) {
                SetTimer(hwnd, 1, 1000, NULL);
            }
            break;

        case WM_TIMER:
            if (wParam == 1) {
                KillTimer(hwnd, 1);
                // 自动启动代理
                if (!isProcessRunning) {
                    GetControlValues();
                    ServerConfig* cfg = GetCurrentServer();
                    if (strlen(cfg->server) > 0 && strlen(cfg->listen) > 0) {
                        StartProcess();
                        AppendLog("[系统] 开机自动启动代理\r\n");
                    }
                }
            }
            break;

        case WM_SYSCOMMAND:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        case WM_TRAYICON:
            if (lParam == WM_LBUTTONUP) {
                if (!IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_RESTORE);
                }
                SetForegroundWindow(hwnd);
                SetActiveWindow(hwnd);
            } 
            else if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                HMENU hMenu = CreatePopupMenu();
                if (hMenu) {
                    AppendMenu(hMenu, MF_STRING, ID_TRAY_OPEN, "打开界面");
                    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "退出程序");
                    SetForegroundWindow(hwnd); 
                    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                    PostMessage(hwnd, WM_NULL, 0, 0);
                    DestroyMenu(hMenu);
                }
            }
            break;

        case WM_APPEND_LOG: {
            char* logText = (char*)lParam;
            if (logText) {
                AppendLog(logText);
                free(logText);
            }
            break;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            HWND hCtrl = (HWND)lParam;
            int ctrlId = GetDlgCtrlID(hCtrl);
            if (ctrlId == ID_LOG_EDIT) {
                SetBkColor(hdcStatic, RGB(255, 255, 255)); 
                SetBkMode(hdcStatic, OPAQUE);              
                return (LRESULT)hBrushLog;                 
            }
            SetBkMode(hdcStatic, TRANSPARENT);             
            return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_TRAY_OPEN:
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                    SetActiveWindow(hwnd);
                    break;
                
                case ID_TRAY_EXIT:
                    if (isProcessRunning) StopProcess();
                    GetControlValues();
                    SaveConfig();
                    RemoveTrayIcon();
                    DestroyWindow(hwnd);
                    break;

                case ID_AUTOSTART_CHECK: {
                    BOOL checked = (SendMessage(hAutoStartCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    
                    if (!IsRunAsAdministrator()) {
                        SendMessage(hAutoStartCheck, BM_SETCHECK, 
                            g_autoStartEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
                        
                        MessageBox(hwnd, 
                            "设置开机启动需要管理员权限。\n\n"
                            "请关闭程序，右键选择\"以管理员身份运行\"后重试。",
                            "需要管理员权限", 
                            MB_OK | MB_ICONWARNING);
                        break;
                    }
                    
                    if (SetAutoStart(checked)) {
                        g_autoStartEnabled = checked;
                        if (checked) {
                            AppendLog("[系统] 已设置开机启动\r\n");
                        } else {
                            AppendLog("[系统] 已取消开机启动\r\n");
                        }
                    } else {
                        SendMessage(hAutoStartCheck, BM_SETCHECK, 
                            g_autoStartEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
                        MessageBox(hwnd, "设置开机启动失败，请确保以管理员权限运行", 
                            "错误", MB_OK | MB_ICONERROR);
                    }
                    break;
                }

                case ID_SERVER_COMBO:
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        if (!isProcessRunning) {
                            GetControlValues();
                            int newIndex = (int)SendMessage(hServerCombo, CB_GETCURSEL, 0, 0);
                            if (newIndex != CB_ERR) {
                                SwitchServer(newIndex);
                            }
                        } else {
                            SendMessage(hServerCombo, CB_SETCURSEL, currentServerIndex, 0);
                            MessageBox(hwnd, "请先停止当前连接后再切换服务器", "提示", MB_OK | MB_ICONWARNING);
                        }
                    }
                    break;

                case ID_SERVER_ADD:
                    if (!isProcessRunning) {
                        AddNewServer();
                    } else {
                        MessageBox(hwnd, "请先停止当前连接", "提示", MB_OK | MB_ICONWARNING);
                    }
                    break;

                case ID_SERVER_DELETE:
                    if (!isProcessRunning) {
                        DeleteCurrentServer();
                    } else {
                        MessageBox(hwnd, "请先停止当前连接", "提示", MB_OK | MB_ICONWARNING);
                    }
                    break;

                case ID_SERVER_RENAME:
                    if (!isProcessRunning) {
                        RenameCurrentServer();
                    } else {
                        MessageBox(hwnd, "请先停止当前连接", "提示", MB_OK | MB_ICONWARNING);
                    }
                    break;

                case ID_START_BTN:
                    if (!isProcessRunning) {
                        GetControlValues();
                        ServerConfig* cfg = GetCurrentServer();
                        if (strlen(cfg->server) == 0) {
                            MessageBox(hwnd, "请输入服务地址 (wss://...)", "提示", MB_OK | MB_ICONWARNING);
                            SetFocus(hServerEdit);
                            break;
                        }
                        if (strlen(cfg->listen) == 0) {
                            MessageBox(hwnd, "请输入监听地址 (127.0.0.1:...)", "提示", MB_OK | MB_ICONWARNING);
                            SetFocus(hListenEdit);
                            break;
                        }
                        SaveConfig();
                        StartProcess();
                    }
                    break;

                case ID_STOP_BTN:
                    if (isProcessRunning) StopProcess();
                    break;

                case ID_CLEAR_LOG_BTN:
                    SetWindowText(hLogEdit, "");
                    break;
            }
            break;

        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;

        case WM_DESTROY:
            RemoveTrayIcon();
            if (hFontUI) DeleteObject(hFontUI);
            if (hFontLog) DeleteObject(hFontLog);
            if (hBrushLog) DeleteObject(hBrushLog);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// 输入对话框消息处理函数
LRESULT CALLBACK InputDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static InputDialogData* pData = NULL;

    switch (uMsg) {
        case WM_CREATE: {
            CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
            pData = (InputDialogData*)cs->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);
            
            int dlgW = Scale(400);
            int dlgH = Scale(160);
            int margin = Scale(20);
            int btnW = Scale(80);
            int btnH = Scale(30);
            int editH = Scale(26);
            
            HWND hPrompt = CreateWindow("STATIC", pData->prompt, 
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                margin, margin, dlgW - margin * 2, Scale(20),
                hwnd, NULL, NULL, NULL);
            SendMessage(hPrompt, WM_SETFONT, (WPARAM)hFontUI, TRUE);
            
            HWND hEdit = CreateWindow("EDIT", pData->buffer,
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                margin, margin + Scale(30), dlgW - margin * 2, editH,
                hwnd, (HMENU)ID_INPUT_EDIT, NULL, NULL);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hFontUI, TRUE);
            SendMessage(hEdit, EM_SETLIMITTEXT, pData->bufferSize - 1, 0);
            SendMessage(hEdit, EM_SETSEL, 0, -1);
            
            HWND hOK = CreateWindow("BUTTON", "确定",
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,
                dlgW - margin - btnW * 2 - Scale(10), dlgH - margin - btnH - Scale(10),
                btnW, btnH,
                hwnd, (HMENU)ID_INPUT_OK, NULL, NULL);
            SendMessage(hOK, WM_SETFONT, (WPARAM)hFontUI, TRUE);
            
            HWND hCancel = CreateWindow("BUTTON", "取消",
                WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON,
                dlgW - margin - btnW, dlgH - margin - btnH - Scale(10),
                btnW, btnH,
                hwnd, (HMENU)ID_INPUT_CANCEL, NULL, NULL);
            SendMessage(hCancel, WM_SETFONT, (WPARAM)hFontUI, TRUE);
            
            SetFocus(hEdit);
            return 0;
        }

        case WM_COMMAND:
            pData = (InputDialogData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            
            switch (LOWORD(wParam)) {
                case ID_INPUT_OK: {
                    HWND hEdit = GetDlgItem(hwnd, ID_INPUT_EDIT);
                    GetWindowText(hEdit, pData->buffer, pData->bufferSize);
                    
                    char* start = pData->buffer;
                    while (*start == ' ' || *start == '\t') start++;
                    char* end = start + strlen(start) - 1;
                    while (end > start && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) end--;
                    *(end + 1) = 0;
                    memmove(pData->buffer, start, strlen(start) + 1);
                    
                    if (strlen(pData->buffer) == 0) {
                        MessageBox(hwnd, "名称不能为空！", "提示", MB_OK | MB_ICONWARNING);
                        SetFocus(hEdit);
                        return 0;
                    }
                    
                    pData->result = TRUE;
                    DestroyWindow(hwnd);
                    return 0;
                }
                
                case ID_INPUT_CANCEL:
                    pData->result = FALSE;
                    DestroyWindow(hwnd);
                    return 0;
            }
            break;

        case WM_CLOSE:
            pData = (InputDialogData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (pData) pData->result = FALSE;
            DestroyWindow(hwnd);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
