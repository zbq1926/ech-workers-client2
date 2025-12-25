#include "app.h"

// 初始化默认服务器配置
void InitDefaultServer()
{
    serverCount = 1;
    currentServerIndex = 0;
    strcpy(servers[0].name, "默认服务器");
    strcpy(servers[0].server, "example.com:443");
    strcpy(servers[0].listen, "127.0.0.1:30000");
    strcpy(servers[0].token, "");
    strcpy(servers[0].ip, "");
    strcpy(servers[0].dns, "");
    strcpy(servers[0].ech, "");
    strcpy(servers[0].pyip, "");
}

// 刷新服务器下拉列表
void RefreshServerCombo()
{
    SendMessage(hServerCombo, CB_RESETCONTENT, 0, 0);
    for (int i = 0; i < serverCount; i++)
    {
        SendMessage(hServerCombo, CB_ADDSTRING, 0, (LPARAM)servers[i].name);
    }
    SendMessage(hServerCombo, CB_SETCURSEL, currentServerIndex, 0);
}

// 切换服务器
void SwitchServer(int index)
{
    if (index < 0 || index >= serverCount)
        return;
    currentServerIndex = index;
    SetControlValues();
    SaveConfig();
    char msg[512];
    sprintf(msg, "[系统] 已切换到服务器: %s\r\n", servers[index].name);
    AppendLog(msg);
}

// 添加新服务器
void AddNewServer()
{
    if (serverCount >= MAX_SERVERS)
    {
        MessageBox(hMainWindow, "服务器数量已达上限", "提示", MB_OK | MB_ICONWARNING);
        return;
    }

    char newName[MAX_NAME_LEN] = "新服务器";
    if (!ShowInputDialog(hMainWindow, "新增服务器", "请输入服务器名称:", newName, MAX_NAME_LEN))
    {
        return;
    }

    for (int i = 0; i < serverCount; i++)
    {
        if (strcmp(servers[i].name, newName) == 0)
        {
            MessageBox(hMainWindow, "服务器名称已存在，请使用其他名称", "提示", MB_OK | MB_ICONWARNING);
            return;
        }
    }

    ServerConfig *newServer = &servers[serverCount];
    if (serverCount > 0)
    {
        memcpy(newServer, &servers[currentServerIndex], sizeof(ServerConfig));
    }
    else
    {
        memset(newServer, 0, sizeof(ServerConfig));
        strcpy(newServer->server, "example.com:443");
        strcpy(newServer->listen, "127.0.0.1:30000");
    }

    strcpy(newServer->name, newName);

    serverCount++;
    currentServerIndex = serverCount - 1;

    RefreshServerCombo();
    SetControlValues();
    SaveConfig();

    char logMsg[512];
    sprintf(logMsg, "[系统] 已添加新服务器: %s\r\n", newName);
    AppendLog(logMsg);
}

// 删除当前服务器
void DeleteCurrentServer()
{
    if (serverCount <= 1)
    {
        MessageBox(hMainWindow, "至少需要保留一个服务器配置", "提示", MB_OK | MB_ICONWARNING);
        return;
    }

    char msg[512];
    sprintf(msg, "确定要删除服务器 \"%s\" 吗？", servers[currentServerIndex].name);
    if (MessageBox(hMainWindow, msg, "确认删除", MB_YESNO | MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    char deletedName[MAX_NAME_LEN];
    strcpy(deletedName, servers[currentServerIndex].name);

    for (int i = currentServerIndex; i < serverCount - 1; i++)
    {
        memcpy(&servers[i], &servers[i + 1], sizeof(ServerConfig));
    }
    serverCount--;

    if (currentServerIndex >= serverCount)
    {
        currentServerIndex = serverCount - 1;
    }

    RefreshServerCombo();
    SetControlValues();
    SaveConfig();

    sprintf(msg, "[系统] 已删除服务器: %s\r\n", deletedName);
    AppendLog(msg);
}

// 重命名当前服务器
void RenameCurrentServer()
{
    char newName[MAX_NAME_LEN];
    strcpy(newName, servers[currentServerIndex].name);

    if (!ShowInputDialog(hMainWindow, "重命名服务器", "请输入新的服务器名称:", newName, MAX_NAME_LEN))
    {
        return;
    }

    for (int i = 0; i < serverCount; i++)
    {
        if (i != currentServerIndex && strcmp(servers[i].name, newName) == 0)
        {
            MessageBox(hMainWindow, "服务器名称已存在，请使用其他名称", "提示", MB_OK | MB_ICONWARNING);
            return;
        }
    }

    char oldName[MAX_NAME_LEN];
    strcpy(oldName, servers[currentServerIndex].name);
    strcpy(servers[currentServerIndex].name, newName);

    RefreshServerCombo();
    SaveConfig();

    char logMsg[1024];
    snprintf(logMsg, sizeof(logMsg), "[系统] 服务器已重命名: %s -> %s\r\n", oldName, newName);
    AppendLog(logMsg);
}

// 获取当前服务器配置
ServerConfig *GetCurrentServer()
{
    if (currentServerIndex >= 0 && currentServerIndex < serverCount)
    {
        return &servers[currentServerIndex];
    }
    return &servers[0];
}

// 显示输入对话框
BOOL ShowInputDialog(HWND parent, const char *title, const char *prompt, char *buffer, int bufferSize)
{
    InputDialogData data;
    data.buffer = buffer;
    data.bufferSize = bufferSize;
    data.prompt = prompt;
    data.result = FALSE;

    int dlgW = Scale(400);
    int dlgH = Scale(160);

    RECT parentRect;
    GetWindowRect(parent, &parentRect);
    int x = parentRect.left + (parentRect.right - parentRect.left - dlgW) / 2;
    int y = parentRect.top + (parentRect.bottom - parentRect.top - dlgH) / 2;

    HWND hDlg = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        "InputDialog",
        title,
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y, dlgW, dlgH,
        parent,
        NULL,
        GetModuleHandle(NULL),
        &data);

    if (!hDlg)
        return FALSE;

    ShowWindow(hDlg, SW_SHOW);
    UpdateWindow(hDlg);

    EnableWindow(parent, FALSE);

    MSG msg;
    while (IsWindow(hDlg) && GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_KEYDOWN)
        {
            if (msg.wParam == VK_RETURN)
            {
                PostMessage(hDlg, WM_COMMAND, ID_INPUT_OK, 0);
                continue;
            }
            else if (msg.wParam == VK_ESCAPE)
            {
                PostMessage(hDlg, WM_COMMAND, ID_INPUT_CANCEL, 0);
                continue;
            }
        }

        if (!IsDialogMessage(hDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);
    SetActiveWindow(parent);

    return data.result;
}

// 获取控件值
void GetControlValues()
{
    ServerConfig *cfg = GetCurrentServer();

    char buf[MAX_URL_LEN];
    GetWindowText(hServerEdit, buf, sizeof(buf));
    strcpy(cfg->server, buf);

    GetWindowText(hListenEdit, buf, sizeof(buf));
    strcpy(cfg->listen, buf);

    GetWindowText(hTokenEdit, cfg->token, sizeof(cfg->token));
    GetWindowText(hIpEdit, cfg->ip, sizeof(cfg->ip));
    GetWindowText(hPyipEdit, cfg->pyip, sizeof(cfg->pyip));
    GetWindowText(hDnsEdit, cfg->dns, sizeof(cfg->dns));
    GetWindowText(hEchEdit, cfg->ech, sizeof(cfg->ech));
}

// 设置控件值
void SetControlValues()
{
    ServerConfig *cfg = GetCurrentServer();
    SetWindowText(hServerEdit, cfg->server);
    SetWindowText(hListenEdit, cfg->listen);
    SetWindowText(hTokenEdit, cfg->token);
    SetWindowText(hIpEdit, cfg->ip);
    SetWindowText(hPyipEdit, cfg->pyip);
    SetWindowText(hDnsEdit, cfg->dns);
    SetWindowText(hEchEdit, cfg->ech);
}
