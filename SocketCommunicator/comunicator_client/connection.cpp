#include<stdio.h>
#include<winsock2.h>
#include<iostream>
#include"connection.h"
#pragma comment(lib,"ws2_32.lib")



bool Transfer::Connect(char *chIpAdress2, int iIpAdressLen2)
{
    if(bWinSockStarted)
    {
        return false;
    }
    char chAdrBuff[iIpAdressLen2+1];
    char chAdrEmrg[10]={'1','2','7','.','0','.','0','.','1','\0',};
    if(NULL==chIpAdress2)
    {
       bConnected=false;
       chIpAdress=&chAdrEmrg[0];
    }
    else
    {
        //Przygotowanie Adrresu

        for(int i=0;i<iIpAdressLen2;i++)
        {
            chAdrBuff[i]=*chIpAdress2;

            chIpAdress2++;
        }
        chAdrBuff[iIpAdressLen2]='\0';
        chIpAdress=&chAdrBuff[0];
        iIpAdressLen=(iIpAdressLen2+1);

        //
    }
    //Initialising Winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&m_wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        bConnected=false;
        return false;
    }

    printf("Initialised.\n");
    bWinSockStarted=true;

    //Create a socket
    if((m_s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
        bConnected=false;
        return false;
    }

    printf("Socket created.\n");


    m_server.sin_addr.s_addr = inet_addr(chIpAdress);
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons( 8888);


    //Connect to remote server
    if (connect(m_s , (struct sockaddr *)&m_server , sizeof(m_server)) < 0)
    {
        puts("connect error");
        bConnected=false;
        return false;
    }

    puts("Connected");
    bConnected=true;
    return true;
}




Transfer::Transfer(char *chIpAdress2, int iIpAdressLen2)
{
    m_chSendThreadData=new char [BUFF_SIZE];
    m_chRecvThreadData=new char [BUFF_SIZE];
    m_bSendThreadRdy=false;
    m_bRecvThreadRdy=false;
    m_bThreadsCleanUp=false;

    bConnected=false;
    bWinSockStarted=false;
    Connect(chIpAdress2, iIpAdressLen2);
    return;
}

Transfer::Transfer(std::string strIpAdress)
{   bConnected=false;
    bWinSockStarted=false;
    if(strIpAdress[0]=='\0')
        bConnected= false;

    int iAdrLen=strIpAdress.length();
    char chAdr[iAdrLen+1];
    /*
    for(int i=0;i<iAdrLen;i++)
    {
        chAdr[i]=strIpAdress[i];
    }
    */
    strcpy(chAdr, &strIpAdress[0]);
    Connect(chAdr,iAdrLen);
    return;
}

bool Transfer::Send(char *chData, int iDataLen)
{
    //std::cout<<"Trans::Send(char*)"<<std::endl;
    if(bConnected==false)
        return false;
    if(iDataLen<=0)
        return false;
    if(NULL==chData)
        return false;

    char chDataAddNULL[iDataLen+1];
    strcpy(chDataAddNULL, chData);
    chDataAddNULL[iDataLen]='\0';

    if( send(m_s , chDataAddNULL , iDataLen+1, 0) < 0)
    {
        puts("Send failed");
        return false;
    }
    puts("Data Send\n");
    return true;
}

bool Transfer::Send(std::string strData)
{
    #ifdef DEBUG1
    std::cout<<"Trans::Send(string)"<<std::endl;
    #endif // DEBUG1
    if(false==bConnected)
    {
        #ifdef DEBUG1
        std::cout<<"Trans::Send(string); bConnected==false "<<std::endl;
        #endif // DEBUG1
        return false;
    }
    if(strData[0]=='\0')
    {
        #ifdef DEBUG1
        std::cout<<"Trans::Send(string); strData[0]=='\0' "<<std::endl;
        #endif // DEBUG1
        return false;
    }


    int iDataLen=0;
    char* chData;
    chData=&strData[0];
    iDataLen=strData.length();
    #ifdef DEBUG1
    std::cout<<"Trans::Send(string); before Send(chData,iDataLen)"<<std::endl;
    #endif // DEBUG1
    return Send(chData,iDataLen);
}


bool Transfer::Recieve(char *chDataBuff, int iBuffSize)
{
    int iRecv_size=0;
    *chDataBuff='\0';
    if(iBuffSize<=0)
        return false;
    if(NULL==chDataBuff)
        return false;
    if(false==bConnected)
    {
        return false;
    }
    if((iRecv_size = recv(m_s , chDataBuff , iBuffSize , 0)) == SOCKET_ERROR)
    {
        puts("recv failed");
        return false;
    }
    if(iRecv_size<0)
    {
        return false;
    }




    puts("Reply received\n");
    *(chDataBuff+iRecv_size)='\0';

    return true;

}

Transfer::~Transfer()
 {
    closesocket(m_s);
    WSACleanup();
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



DWORD WINAPI ClientSendThread(void *cln)
{
    Transfer *pclient;
    pclient= (Transfer*)cln;



    while(1)
    {
        if(pclient->m_bSendThreadRdy)// jesli sa dane w buforze do wyslania
        {
            *(pclient->m_chSendThreadData+pclient->m_iSendDataSize)='\0';
            if( send( pclient->m_s ,pclient->m_chSendThreadData ,pclient->m_iSendDataSize, 0) < 0)
            {
                puts("Send failed");
                return false;
            }

            pclient->m_bSendThreadRdy=false;
            ClearBuff(pclient->m_chSendThreadData,BUFF_SIZE);



        }//end if(m_bSendThreadRdy)
        if(pclient->m_bThreadsCleanUp)
        {
            ExitThread(NULL);
            return 0;
        }


    }//end while


    return 0;
}




bool Transfer::PrepToSend(char * chBuff,int iDataLen)
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



bool Transfer::PrepToSend(char* chInputBuff)
{
    return PrepToSend(chInputBuff,strlen(chInputBuff));
}


void Transfer::Send()
{
    m_bSendThreadRdy=true;
}


DWORD WINAPI ClientRecvThread(void* serv)
{
    Transfer *pclient;
    pclient= (Transfer*)serv;

    ClearBuff(pclient->m_chRecvThreadData,BUFF_SIZE);
    int iRecv_size=0;

    while(!(pclient->m_bThreadsCleanUp))
    {


        if((iRecv_size = recv(pclient->m_s , pclient->m_chRecvThreadData , BUFF_SIZE , 0)) == SOCKET_ERROR)
        {
            puts("recv failed hue \n");
        }


        //else
        //{
            printf("response: %s \n",pclient->m_chRecvThreadData);
        //}
        ClearBuff(pclient->m_chRecvThreadData,BUFF_SIZE);







    }
    ExitThread(NULL);
    return 0;

}




















































