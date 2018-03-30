#include <iostream>
#include "server_class.h"
#include <conio.h>
#include <windows.h>
#include <tchar.h>
//#include <strsafe.h>

using namespace std;
#ifndef BUFF_SIZE
    #define BUFF_SIZE 1000
#endif // BUFF_SIZE
#define NUM_THREADS 2






int main()
{
    HANDLE hndSendThread, hndRecvThread;
    DWORD dwSendThreadID=0,dwRecvThreadID=0;




    char chHue[100];
    char chSendBuff[BUFF_SIZE];
    char chRecvBuff[BUFF_SIZE];
    char chStash='\0';
    int iSendBuffP=0,iRecvBuffP=0;
    ClearBuff(chRecvBuff,BUFF_SIZE);
    bool bExitLoop=false;






    Server test;
    Server *testpointier;
    testpointier=&test;
    void * pv=testpointier;

    Server * testVP = static_cast<Server*>(pv);







    testpointier=&test;
    test.Listen();
    test.Accept();
    //std::cout<<"test.Recieve(chHue,100) : "<<test.Recieve(chHue,100)<<std::endl;



    hndSendThread= CreateThread(NULL,0,SendThread,(void *)(&test),0 ,&dwSendThreadID);
    hndRecvThread= CreateThread(NULL,0,RecvThread,(void *)(&test),0 ,&dwRecvThreadID);




    test.PrepToSend("dzienhuehueuhe");
    test.Send();

    while(1)
    {


        gets(chSendBuff);

        test.PrepToSend(chSendBuff);
        test.Send();





    }










    while(1)
    {
        if(kbhit())
            break;
    }











/*
    while(!bExitLoop)
    {

    }


    std::cout<<chHue<<endl;
*/
}//end main()
