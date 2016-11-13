/*
 * File:   main.cpp
 * Author: dontsov
 *
 * Created on November 9, 2016, 10:38 PM
 */

#include <memory>
#include <algorithm>
#include <cassert>

#include "sys/socket.h"
#include "arpa/inet.h"
#include "netdb.h"

#include "SocketAddress.hpp"
#include "SocketAddressFactory.hpp"
#include "SocketUtil.hpp"

using namespace std;

const size_t BUFFER_SIZE = 1024;

int main(int argc, char** argv) {
#ifdef SERVER
    /* ********************* UDP SERVER ********************* */
    // 1. Создать UDP socket
    auto udpSocket = SocketUtil::CreateUDPSocket(INET);
    assert(udpSocket);
    // 2. Создать socket_address
    auto serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:7891");
    assert(serverAddress);
    // 3. Связать UDP socket с socket_address
    int error = udpSocket->Bind(*serverAddress);
    if (error) {
        exit(1);
    }
    // 4. Цикл UDP сервера
    char buffer[BUFFER_SIZE];
    bool IsUDPServerRun = true;
    SocketAddressPtr clientAddress = std::make_shared<SocketAddress>();
    while (IsUDPServerRun) {
        printf("UDP Server Running\n");
        //      Получить данные
        const int readByteCount = udpSocket->ReceiveFrom(buffer, BUFFER_SIZE, *clientAddress);
        if (readByteCount < 0) {
            exit(1);
        }
        //      Обработать данные
        for (int i = 0; i < readByteCount; ++i) {
            buffer[i] = toupper(buffer[i]);
        }
        printf("Received data from client: %s", buffer);
        //      Отправить обработанные данные клиенту.
        const int sendByteCount = udpSocket->SendTo(buffer, readByteCount, *clientAddress);
    }
#else
    /* ********************* UDP CLIENT ********************* */
    // 1. Создать UDP socket
    auto udpSocket = SocketUtil::CreateUDPSocket(INET);
    assert(udpSocket);
    // 2. Задать SocketAddress сервера
    auto serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:7891");
    assert(serverAddress);
    // 4. Цикл UDP клиента
    char buffer[BUFFER_SIZE];
    bool IsUDPClientRun = true;
    while (IsUDPClientRun) {
        printf("Type a sentence to send to server:\n");
        fgets(buffer, BUFFER_SIZE, stdin);
        printf("You typed: %s", buffer);

        //      Отправить данные серверу
        const int sendByteCount = udpSocket->SendTo(buffer, BUFFER_SIZE, *serverAddress);
    }
#endif
    return 0;
}