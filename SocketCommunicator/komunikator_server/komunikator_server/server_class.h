#ifndef SERVER_CLASS_H_INCLUDED
#define SERVER_CLASS_H_INCLUDED

#include<stdio.h>
#include<winsock2.h>
#include<iostream>
#include <windows.h>
#include <tchar.h>
//#include <strsafe.h>

#pragma comment(lib,"ws2_32.lib")

class Server
{
    private:
    bool m_BindFlag;
    bool m_bConnectionFlag;
    WSADATA m_wsa;
    SOCKET m_s, m_ListenSocket;
    struct sockaddr_in m_server , m_client;
    int m_iSockAddSize;

    char* m_chSendThreadData;
    char *m_chRecvThreadData;
    bool m_bSendThreadRdy, m_bRecvThreadRdy,m_bThreadsCleanUp;
    int m_iSendDataSize;






    public:

    Server();
    ~Server();
    bool Listen();
    bool Accept();
    bool Send(std::string strData);
    bool Send(char *chData, int iDataLen);
    void Send();//send using SendThread
    bool Recieve(char *chDataBuff, int iBuffSize);
    bool PeekABoo(char *chDataBuff, int iBuffSize);
    bool PrepToSend(char *chData, int iDataLen);
    //bool PrepToSend(std::string strData);
    bool PrepToSend(char * chData);


    friend DWORD WINAPI SendThread(void *);
    friend DWORD WINAPI RecvThread(void *);
};


void ClearBuff(char* chBuff,int iLen);
DWORD WINAPI SendThread(void *);
DWORD WINAPI RecvThread(void *);




#endif // SERVER_CLASS_H_INCLUDED
