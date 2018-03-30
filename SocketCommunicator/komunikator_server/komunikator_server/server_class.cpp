#include "server_class.h"
#include<stdio.h>
#include<winsock2.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")
#include<iostream>



#define BUFF_SIZE 1000

using std::string;

Server::Server()
{
    m_chSendThreadData=new char [BUFF_SIZE];
    m_chRecvThreadData=new char [BUFF_SIZE];
    m_bSendThreadRdy=false;
    m_bRecvThreadRdy=false;
    m_bThreadsCleanUp=false;

     printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&m_wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        m_BindFlag=false;
        return;
    }

    printf("Initialised.\n");

    //Create a socket
    if((m_s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    m_server.sin_family = AF_INET;
    m_server.sin_addr.s_addr = INADDR_ANY;
    m_server.sin_port = htons( 8888 );

    //Bind
    if( bind(m_s ,(struct sockaddr *)&m_server , sizeof(m_server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
    }

    puts("Bind done");
    m_BindFlag=true;
    return;
}


bool Server::Listen()
{
//Listen to incoming connections
    listen(m_s , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    return true;
}

bool Server::Accept()
{
    m_iSockAddSize = sizeof(struct sockaddr_in);
    std::cout<<"accepting all connections "<<std::endl;
    m_ListenSocket = accept(m_s , (struct sockaddr *)&m_client, &m_iSockAddSize);
    if (m_ListenSocket == INVALID_SOCKET)
    {
        printf("accept failed with error code : %d" , WSAGetLastError());
        return false;
    }

    puts("Connection accepted");
    m_bConnectionFlag=true;
    return true;
}




bool Server::Send(char *chData, int iDataLen)
{
    if(iDataLen<=0)
        return false;
    if(NULL==chData)
        return false;
    if(false==m_bConnectionFlag)
        return false;
    if( send(m_ListenSocket , chData , iDataLen, 0) < 0)
    {
        puts("Send failed");
        return false;
    }
    puts("Data Send\n");
    return true;
}

bool Server::Send(string strData)
{
    if(strData[0]=='\0')
    {
        return false;
    }


    int iDataLen=0;
    char* chData;
    chData=&strData[0];
    iDataLen=strData.length();
    return Send(chData,iDataLen);
}

bool Server::Recieve(char *chDataBuff, int iBuffSize)
{
    int iRecv_size=0;
    if(iBuffSize<=0)
        return false;
    if(NULL==chDataBuff)
        return false;
    if(false==m_bConnectionFlag)
    {
        return false;
    }
    if((iRecv_size = recv(m_ListenSocket , chDataBuff , iBuffSize , 0)) == SOCKET_ERROR)
    {
        puts("recv failed");
        return false;
    }
    if(iRecv_size>=iBuffSize)
        return false;

    puts("Reply received\n");
    *(chDataBuff+iRecv_size)='\0';

    return true;

}

bool Server::PeekABoo(char *chDataBuff, int iBuffSize)
{
    int iRecv_size=0;
    if(iBuffSize<=0)
        return false;
    if(NULL==chDataBuff)
        return false;
    if(false==m_bConnectionFlag)
    {
        return false;
    }
    if((iRecv_size = recv(m_ListenSocket , chDataBuff , iBuffSize , MSG_PEEK)) == SOCKET_ERROR)
    {
        puts("recv failed");
        return false;
    }
    if(iRecv_size>=iBuffSize)
        return false;

    *(chDataBuff+iRecv_size)='\0';

    return true;

}



Server::~Server()
{
    closesocket(m_s);
    WSACleanup();

    delete m_chSendThreadData;
    delete m_chRecvThreadData;
}




DWORD WINAPI SendThread(void *serv)
{
    Server *pserver;
    pserver= (Server*)serv;



    while(1)
    {
        if(pserver->m_bSendThreadRdy)// jesli sa dane w buforze do wyslania
        {
            *(pserver->m_chSendThreadData+pserver->m_iSendDataSize)='\0';
            if( send( pserver->m_ListenSocket ,pserver->m_chSendThreadData ,pserver->m_iSendDataSize, 0) < 0)
            {
                puts("Send failed");
                return false;
            }

            pserver->m_bSendThreadRdy=false;
            ClearBuff(pserver->m_chSendThreadData,BUFF_SIZE);



        }//end if(m_bSendThreadRdy)
        if(pserver->m_bThreadsCleanUp)
        {
            ExitThread(NULL);
            return 0;
        }


    }//end while


    return 0;
}






void ClearBuff(char* chBuff,int iLen)
{
    char* chPoint;
    chPoint=chBuff;
    for(int i=0;i<iLen;i++)
    {
        (*chPoint)='0';
        chPoint++;
    }
}





bool Server::PrepToSend(char * chBuff,int iDataLen)
{
    if(iDataLen>BUFF_SIZE)
    {
        iDataLen=BUFF_SIZE-1;
    }



    ClearBuff(m_chSendThreadData,BUFF_SIZE);
    if(m_bSendThreadRdy) // to znaczy ze watek wysylania jeszcze nie oproznil bufforu
        return false;

    strncpy(m_chSendThreadData,chBuff,iDataLen);
    *(m_chSendThreadData+iDataLen)='\0';
    m_iSendDataSize=iDataLen;

    return true;
}



bool Server::PrepToSend(char* chInputBuff)
{
    return PrepToSend(chInputBuff,strlen(chInputBuff));
}






/*
bool Server::PrepToSend(string strInput)
{

    printf("przed  if(m_bSendThreadRdy) \n");
    if(m_bSendThreadRdy) // to znaczy ze watek wysylania jeszcze nie oproznil bufforu
        return false;
    printf("przed for(int i=0;i<strInput.length();i++) \n");



    printf("strInput.length() : %d \n",strInput.length());
    for(int i=0;i<strInput.length();i++)
    {
        printf("przed *(m_chSendThreadData+i)=strInput[i]; i = %d\n", i);
        *(m_chSendThreadData+i)=strInput[i];
    }
    printf("przed *(m_chSendThreadData+strInput.length())='\0'; \n");
    *(m_chSendThreadData+strInput.length())='\0';
    printf("przed m_iSendDataSize=strInput.length(); \n");
    m_iSendDataSize=strInput.length();

    return true;
}

*/
void Server::Send()
{
    m_bSendThreadRdy=true;
}


DWORD WINAPI RecvThread(void* serv)
{
    Server *pserver;
    pserver= (Server*)serv;

    ClearBuff(pserver->m_chRecvThreadData,BUFF_SIZE);
    int iRecv_size=0;

    while(!(pserver->m_bThreadsCleanUp))
    {


        if((iRecv_size = recv(pserver->m_ListenSocket , pserver->m_chRecvThreadData , BUFF_SIZE , 0)) == SOCKET_ERROR)
        {
            puts("recv failed hue \n");
        }


        //else
        //{
            printf("response: %s \n",pserver->m_chRecvThreadData);
        //}
        ClearBuff(pserver->m_chRecvThreadData,BUFF_SIZE);







    }
    ExitThread(NULL);
    return 0;

}





























