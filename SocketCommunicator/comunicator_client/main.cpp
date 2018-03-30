#include <iostream>
#include <conio.h>
#include <tchar.h>
#include <windows.h>
#include <cstdio>
#include"connection.h"
using namespace std;


#ifndef BUFF_SIZE
    #define BUFF_SIZE 1000
#endif // BUFF_SIZE

int main()
{
    char chHue[BUFF_SIZE];
    ClearBuff(chHue,BUFF_SIZE);
    char chBuff[50];
    std::string strInput, strSend;
    HANDLE hndSendThread, hndRecvThread;
    DWORD dwSendThreadID,dwRecvThreadID;

    /*
    std::getline(std::cin,strInput);
    std::cout<<strInput<<std::endl;
    for(int i=0;i<strInput.length()+1;i++)
        std::cout<<strInput[i]<<" numer kodu ascii"<<int(strInput[i])<<endl;
    /*
    for(int i=0;i<strInput.length();i++)
    {
        chBuff[i]=strInput[i];
    }




    //Transfer trsSite(chBuff, strInput.length());
    Transfer trsSite(strInput);*/


    Transfer trsSite("192.168.19.100");





    hndSendThread= CreateThread(NULL,0,ClientSendThread,(void *)(&trsSite),0 ,&dwSendThreadID);
    hndRecvThread= CreateThread(NULL,0,ClientRecvThread,(void *)(&trsSite),0 ,&dwRecvThreadID);

    while(1)
    {
        gets(chHue);
        trsSite.PrepToSend(chHue);
        trsSite.Send();

    }

}
