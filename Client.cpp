#include "Client.h"

#include <stdio.h>
#include <winsock2.h>
#include "Tools.h"

#define packetloss 0

const int CLIENT_PORT=42043; 

struct sockaddr_in serverAddr; 
struct sockaddr_in clientAddr; 


sockaddr_in clientService;


int Client::Setup(char *ip, int port){

  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

  if (iResult != NO_ERROR){
     ALERTN("Client: Error at WSAStartup().\n");
     return 1;
  }


  clientSock = socket(AF_INET, SOCK_DGRAM, 0);

  if (clientSock == INVALID_SOCKET) {
    ALERTM("Client: socket() - Error at socket()\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  clientService.sin_family = AF_INET;
  
  if(ip == 0){
    char ip2[40];
      
    gethostname(ip2, 32);
    struct hostent *hp = gethostbyname(ip2);
    sprintf(ip2, "%u.%u.%u.%u", hp->h_addr_list[0][0], hp->h_addr_list[0][1], hp->h_addr_list[0][2],hp->h_addr_list[0][3]);

    clientService.sin_addr.s_addr = inet_addr(ip2);


  }else{

  clientService.sin_addr.s_addr = inet_addr(ip);
  }
  clientService.sin_port = htons(port);

  connected = -1;


  return 0;
}


void Client::Release(){
   closesocket(clientSock);
   WSACleanup();
}

int first = 10;

void Client::SendMsg(char *str, int s){
#if packetloss > 0
  if(rand()%packetloss != 0 && first-- < 0) return;
#endif
 TYPE *t = (TYPE*)(str+s);
 *t = MSG_FIN;
 s += sizeof(TYPE);
 sendto(clientSock, str, s, 0, (SOCKADDR *)&clientService, sizeof(clientService));
}


void Client::SendMsg2(char *str, int s){
#if packetloss > 0

  if(rand()%packetloss != 0 && first-- < 0) return;
#endif

  sendto(clientSock, str, s, 0, (SOCKADDR *)&clientService, sizeof(clientService));
}


int Client::RcvMsg(char *str){

  return (int)recv(clientSock, str, 4096, 0);
 
}