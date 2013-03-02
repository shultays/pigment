#include <stdio.h>
#include <winsock2.h>
#include <mmsystem.h>
#include <Windows.h>

#include "Server.h"

int main(){
  WORD wVersionRequested;
  WSADATA wsaData;
  int wsaerr;
  wVersionRequested = MAKEWORD(2, 2);

  wsaerr = WSAStartup(wVersionRequested, &wsaData);

  if (wsaerr != 0){
    printf("Server: The Winsock dll not found!\n");
    return 0;
  }

  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 ){
    printf("Server: The dll do not support the Winsock version %u.%u!\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
    WSACleanup();
    return 0;
  }

  SOCKET s_socket;
  s_socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (s_socket == INVALID_SOCKET){
    printf("Server: Error at socket(): %ld\n", WSAGetLastError());
    WSACleanup();
    return 0;
  }
  sockaddr_in service;
  service.sin_family = AF_INET;


  char ip[32];
  gethostname(ip, 32);
  struct hostent *hp = gethostbyname(ip);
  printf("%s\n", ip);

  if(hp != NULL){
    printf("%d.%d.%d.%d\n", (hp->h_addr_list[0][0]+256)%256,(hp->h_addr_list[0][1]+256)%256,(hp->h_addr_list[0][2]+256)%256,(hp->h_addr_list[0][3]+256)%256);
  
    service.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
    service.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
    service.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
    service.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];

  }else{
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
  }
  service.sin_port = htons(15001);

  if (bind(s_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR){
    printf("Server: bind() failed: %ld.\n", WSAGetLastError());
    closesocket(s_socket);
    return 0;
  }

  unsigned long int nonBlockingMode = 1;

  ioctlsocket( s_socket, FIONBIO, &nonBlockingMode );

  printf("Ready to Rock\n");

  StartServer(s_socket);

  WSACleanup();

  return 0;
}
