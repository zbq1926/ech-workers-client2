#include "app.h"

// 异步添加日志
void AppendLogAsync(const char* text) {
    if (!text) return;
    char* msgCopy = strdup(text); 
    if (msgCopy) {
        if (!PostMessage(hMainWindow, WM_APPEND_LOG, 0, (LPARAM)msgCopy)) {
            free(msgCopy);
        }
    }
}

// 添加日志
void AppendLog(const char* text) {
    if (!IsWindow(hLogEdit)) return;
    
    // 限制日志长度，避免内存占用过大
    int currentLen = GetWindowTextLength(hLogEdit);
    if (currentLen > 80000) {
        SendMessage(hLogEdit, WM_SETREDRAW, FALSE, 0);
        SendMessage(hLogEdit, EM_SETSEL, 0, 32000);
        SendMessage(hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)"");
        SendMessage(hLogEdit, WM_SETREDRAW, TRUE, 0);
        InvalidateRect(hLogEdit, NULL, TRUE);
    }
    
    // 在末尾添加新日志
    int len = GetWindowTextLength(hLogEdit);
    SendMessage(hLogEdit, EM_SETSEL, len, len);
    SendMessage(hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
}
