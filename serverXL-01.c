#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

int __cdecl main() {
    int result;

    WSADATA wsadata;
    result = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
    }

    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
        printf("Could not find the suitable version for this dll. Please make sure you have the 2.2 version\n");
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("socket function failed with error %d\n", WSAGetLastError());
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    result = bind(sock, (const struct sockaddr*)&addr, sizeof(addr));
    if (result == SOCKET_ERROR) {
        printf("bind function failed with error %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    result = listen(sock, 10);
    if (result == SOCKET_ERROR) {
        printf("listen function failed with error %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    SOCKET client = accept(sock, 0, 0);
    if (client == INVALID_SOCKET) {
        printf("accept function failed with error %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    char request[256] = { 0 };
    result = recv(client, request, sizeof(request), 0);
    if (result == SOCKET_ERROR) {
        printf("recv function failed with error %d\n", WSAGetLastError());
        closesocket(client);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    if (memcmp(request, "GET /", 5) == 0) {
        FILE* f = fopen("index.html", "r");
        if (f != NULL) {
            char buffer[256] = { 0 };
            fread(buffer, 1, sizeof(buffer), f);
            send(client, buffer, sizeof(buffer), 0);
            fclose(f);
        }
    }

    result = closesocket(client);
    if (result == SOCKET_ERROR) {
        printf("closesocket function failed with error %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    result = closesocket(sock);
    if (result == SOCKET_ERROR) {
        printf("closesocket function failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    WSACleanup();

    return 0;
}