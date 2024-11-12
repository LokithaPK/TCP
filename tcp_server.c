#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8081

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_socket1, client_socket2;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock Initialized.\n");

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Start listening for client connections
    if (listen(server_fd, 2) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Waiting for clients to connect...\n");

    // Accept the first client connection
    if ((client_socket1 = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    printf("Client 1 connected.\n");

    // Accept the second client connection
    if ((client_socket2 = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    printf("Client 2 connected.\n");

    // Main loop to handle chat
    while (1) {
        // Read message from client 1 and send it to client 2
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket1, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            printf("Client 1 disconnected.\n");
            break;
        }
        printf("Client 1: %s\n", buffer);  // Display on server
        send(client_socket2, buffer, strlen(buffer), 0);  // Send to client 2

        // Read message from client 2 and send it to client 1
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket2, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            printf("Client 2 disconnected.\n");
            break;
        }
        printf("Client 2: %s\n", buffer);  // Display on server
        send(client_socket1, buffer, strlen(buffer), 0);  // Send to client 1
    }

    // Close sockets and clean up
    closesocket(client_socket1);
    closesocket(client_socket2);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
