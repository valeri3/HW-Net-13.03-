#include "header.h"

bool isValidName(const char* name)
{
    for (int i = 0; name[i] != '\0'; i++)
    {
        if (isspace(name[i]))
        {
            return false;
        }
    }

    return true;
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

    sockaddr_in addrTo;
    addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(23000);
    inet_pton(AF_INET, "127.0.0.1", &addrTo.sin_addr.s_addr);

    const size_t sendBufSize = 1024;
    char sendBuf[sendBufSize];
    char originalSendBuf[sendBufSize];

    do 
    {
        cout << "Введите данные (никнейм, номер от 0 до 15, текст): ";
       
        cin.getline(sendBuf, sendBufSize);
        strcpy_s(originalSendBuf, sendBuf);

        char nickname[sendBufSize], numberStr[sendBufSize];

        char* nextToken;
        char* token = strtok_s(sendBuf, " ", &nextToken);
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

            partsCount++;
            token = strtok_s(NULL, " ", &nextToken);
        }

        if (partsCount != 3) 
        {
            cout << "Ошибка: Введите никнейм, цифру от 0 до 15 и текст, разделённые пробелами\n";
            
            continue;
        }

        if (!isValidName(nickname)) 
        {
            cout << "Ошибка: никнейм должен быть цельным словом без пробелов\n";
            
            continue;
        }

        int number = atoi(numberStr);
        if (number < 0 || number > 15) 
        {
            cout << "Ошибка: вторая часть должна быть цифрой от 0 до 15\n";
            
            continue;
        }

        break;

    } while (true);

    cout << "Отправка данных..." << endl;

    int sendResult = sendto(udpSocket, originalSendBuf, strlen(originalSendBuf), 0, (SOCKADDR*)&addrTo, sizeof(addrTo));
    
    if (sendResult == SOCKET_ERROR)
    {
        cout << "Отправка не удалась " << WSAGetLastError() << endl;
       
        return 4;
    }

    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];

    sockaddr_in addrFrom;
    int addrFromSize = sizeof(addrFrom);

    cout << "Получение данных..." << endl;
    int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&addrFrom, &addrFromSize);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "Получение данных не вышло " << WSAGetLastError() << endl;
        
        return 4;
    }

    receiveBuf[bytesReceived] = '\0';
    cout << "Получено от " << addrFrom.sin_addr.S_un.S_addr << endl;
    cout << "Данные: " << receiveBuf << endl;

    closesocket(udpSocket);
    WSACleanup();
}