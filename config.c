#include "app.h"

// 保存配置到文件
void SaveConfig() {
    char configPath[MAX_PATH];
    GetConfigFilePath(configPath, MAX_PATH);
    
    FILE* f = fopen(configPath, "w");
    if (!f) return;
    
    fprintf(f, "[Settings]\n");
    fprintf(f, "current_server=%d\n", currentServerIndex);
    fprintf(f, "server_count=%d\n\n", serverCount);
    
    for (int i = 0; i < serverCount; i++) {
        fprintf(f, "[Server%d]\n", i);
        fprintf(f, "name=%s\n", servers[i].name);
        fprintf(f, "server=%s\n", servers[i].server);
        fprintf(f, "listen=%s\n", servers[i].listen);
        fprintf(f, "token=%s\n", servers[i].token);
        fprintf(f, "ip=%s\n", servers[i].ip);
        fprintf(f, "dns=%s\n", servers[i].dns);
        fprintf(f, "ech=%s\n", servers[i].ech);
        fprintf(f, "pyip=%s\n\n", servers[i].pyip);
    }
    
    fclose(f);
}

// 从文件加载配置
void LoadConfig() {
    char configPath[MAX_PATH];
    GetConfigFilePath(configPath, MAX_PATH);
    
    FILE* f = fopen(configPath, "r");
    if (!f) return;
    
    char line[MAX_URL_LEN];
    int currentSection = -1;
    
    while (fgets(line, sizeof(line), f)) {
        char* newline = strchr(line, '\n');
        if (newline) *newline = 0;
        
        if (line[0] == 0 || line[0] == ';' || line[0] == '#') continue;
        
        if (line[0] == '[') {
            if (strncmp(line, "[Settings]", 10) == 0) {
                currentSection = -1;
            } else if (strncmp(line, "[Server", 7) == 0) {
                int idx;
                if (sscanf(line, "[Server%d]", &idx) == 1) {
                    currentSection = idx;
                }
            }
            continue;
        }
        
        char* val = strchr(line, '=');
        if (!val) continue;
        *val++ = 0;
        
        if (currentSection == -1) {
            if (strcmp(line, "current_server") == 0) {
                currentServerIndex = atoi(val);
            } else if (strcmp(line, "server_count") == 0) {
                serverCount = atoi(val);
                if (serverCount > MAX_SERVERS) serverCount = MAX_SERVERS;
                if (serverCount < 0) serverCount = 0;
            }
        } else if (currentSection >= 0 && currentSection < MAX_SERVERS) {
            ServerConfig* srv = &servers[currentSection];
            if (strcmp(line, "name") == 0) {
                strncpy(srv->name, val, MAX_NAME_LEN - 1);
                srv->name[MAX_NAME_LEN - 1] = 0;
            } else if (strcmp(line, "server") == 0) {
                strncpy(srv->server, val, MAX_URL_LEN - 1);
            } else if (strcmp(line, "listen") == 0) {
                strncpy(srv->listen, val, MAX_SMALL_LEN - 1);
            } else if (strcmp(line, "token") == 0) {
                strncpy(srv->token, val, MAX_URL_LEN - 1);
            } else if (strcmp(line, "ip") == 0) {
                strncpy(srv->ip, val, MAX_SMALL_LEN - 1);
            } else if (strcmp(line, "dns") == 0) {
                strncpy(srv->dns, val, MAX_SMALL_LEN - 1);
            } else if (strcmp(line, "ech") == 0) {
                strncpy(srv->ech, val, MAX_SMALL_LEN - 1);
            } else if (strcmp(line, "pyip") == 0) {
                strncpy(srv->pyip, val, MAX_SMALL_LEN - 1);
            }
        }
    }
    
    fclose(f);
    
    // 验证配置的有效性
    if (currentServerIndex < 0 || currentServerIndex >= serverCount) {
        currentServerIndex = 0;
    }
}
