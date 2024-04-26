#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Failed to create socket\n";
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error: Bind failed\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error: Listen failed\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket == -1) {
        std::cerr << "Error: Accept failed\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ":" 
              << ntohs(clientAddr.sin_port) << "\n";

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while (true) {
        bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead == -1) {
            std::cerr << "Error: Failed to receive data\n";
            break;
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        std::cout << "Received message from client: " << std::string(buffer, bytesRead) << "\n";

        const char *ackMsg = "Message received successfully";
        if (send(clientSocket, ackMsg, strlen(ackMsg), 0) == -1) {
            std::cerr << "Error: Failed to send acknowledgment\n";
            break;
        }
    }

    close(clientSocket);
    close(serverSocket);

    return 0;
}
