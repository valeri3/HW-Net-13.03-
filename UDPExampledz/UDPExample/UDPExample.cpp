#include "header.h"

void replaceForbiddenWords(char* message)
{
    const char* forbiddenWords[] = { "end", "start", "error", "hello" };

    for (const auto& word : forbiddenWords)
    {
        for (char* pos = message; (pos = strstr(pos, word)) != nullptr; pos += strlen(word))
        {
            fill(pos, pos + strlen(word), '*');
        }

    }
}

void printHighlightedText(const char* text)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

    for (const char* i = text; *i; ++i)
    {
        if (*i == '*')
        {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);

            cout << *i;

            SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
        }
        else
        {
            cout << *i;
        }
    }
}

int main()
{
    // Устанавливаем кодировку UTF-8 для консоли
    SetConsoleOutputCP(CP_UTF8);

    // Устанавливаем русскую локаль
    setlocale(LC_ALL, "Russian");

    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup  с ошибкой " << res << endl;

        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "Ошибка создания сокета " << WSAGetLastError() << endl;

        return 2;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;    
    addr.sin_port = htons(23000);   
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    if (bind(udpSocket, (SOCKADDR*)&addr, sizeof(addr)) != NO_ERROR)
    {
        cout << "Ошибка бинда " << WSAGetLastError() << endl;

        return 3;
    }

    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];

    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    cout << "Получение данных..." << endl;

    int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "получение не удалось " << WSAGetLastError() << endl;
        
        return 4;
    }

    receiveBuf[bytesReceived] = '\0';

    char nickname[receiveBufSize], numberStr[4], text[receiveBufSize];

    do 
    {
        char* nextToken;
        char* token = strtok_s(receiveBuf, " ", &nextToken);
        int partsCount = 0;

        while (token != NULL && partsCount < 3) 
        {
            if (partsCount == 0)
            {
                strcpy_s(nickname, token);
            }
            else if (partsCount == 1)
            { 
                strcpy_s(numberStr, token);
            }
            else if (partsCount == 2)
            {
                strcpy_s(text, token);
                strcat_s(text, " ");
                strcat_s(text, nextToken);
                break;
            }

            partsCount++;
            token = strtok_s(NULL, " ", &nextToken);
        }

        break;

    } while (true);

    replaceForbiddenWords(text);

    cout << "Received from " << senderAddr.sin_addr.S_un.S_addr << endl;

    COORD point;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(h, &screenBufferInfo);

    point = screenBufferInfo.dwCursorPosition;

    SetConsoleCursorPosition(h, point);
    SetConsoleTextAttribute(h, atoi(numberStr));

    cout << nickname << ": ";

    printHighlightedText(text);

    cout << endl;

    SetConsoleTextAttribute(h, 7);

    const size_t sendBufSize = 1024;
    char sendBuf[sendBufSize] = "Привет от UDP сервера";

    int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&senderAddr, senderAddrSize);

    if (sendResult == SOCKET_ERROR)
    {
        cout << "отправка не удалась " << WSAGetLastError() << endl;
        
        return 4;
    }

    closesocket(udpSocket);
    WSACleanup();
}