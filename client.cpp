#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;
const char *SERVER_IP = "127.0.0.1";

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error: Failed to create socket\n";
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported\n";
        close(clientSocket);
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Connection failed\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to server\n";

    while (true) {
        std::cout << "Enter message to send (or type 'exit' to quit): ";
        std::string message;
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        if (send(clientSocket, message.c_str(), message.length(), 0) == -1) {
            std::cerr << "Error: Failed to send message\n";
            break;
        }

        std::cout << "Message sent to server: " << message << "\n";

        char buffer[BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead == -1) {
            std::cerr << "Error: Failed to receive acknowledgment\n";
            break;
        } else if (bytesRead == 0) {
            std::cerr << "Connection closed by server\n";
            break;
        } else {
            buffer[bytesRead] = '\0';
            std::cout << "Received acknowledgment from server: " << buffer << "\n";
        }
    }

    close(clientSocket);

    return 0;
}

