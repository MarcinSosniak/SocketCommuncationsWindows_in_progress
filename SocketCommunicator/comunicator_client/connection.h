#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include<iostream>
#include<winsock2.h>
#include<windows.h>

#ifndef BUFF_SIZE
    #define BUFF_SIZE 1000
#endif // BUFF_SIZE



#pragma comment(lib,"ws2_32.lib")

class Transfer
{
    //public://remove later
    private:
    bool  bConnected;
    bool  bWinSockStarted;

    char* m_chSendThreadData;
    char *m_chRecvThreadData;
    int m_iSendDataSize;

    bool m_bSendThreadRdy;
    bool m_bRecvThreadRdy;
    bool m_bThreadsCleanUp;


    WSADATA m_wsa;
    SOCKET m_s;
    struct sockaddr_in m_server;
    char *chIpAdress;
    int iIpAdressLen;

    public:
    ~Transfer();
    Transfer(std::string strIpAdress);
    Transfer(char *chIpAdress2, int iIpAdressLen2);
    bool Connect(char *chIpAdress2, int iIpAdressLen2);
    bool Send(std::string strData);//ads NULL byte
    bool Send(char *chData, int iDataLen);//ads NULL byte
    void Send();
    bool Recieve(char *chDataBuff, int iBuffSize);

    bool GetConnected() {return bConnected;}

    bool PrepToSend(char *chData, int iDataLen);
    //bool PrepToSend(std::string strData);
    bool PrepToSend(char * chData);

    friend DWORD WINAPI ClientSendThread(void *);
    friend DWORD WINAPI ClientRecvThread(void *);



};
void ClearBuff(char* chBuff,int iLen);
DWORD WINAPI ClientSendThread(void *);
DWORD WINAPI ClientRecvThread(void *);

#endif // CONNECTION_H_INCLUDED
