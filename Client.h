#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkStructs.h"

class Client{
public:
  int Setup(char *ip, int port);

  void SendMsg(char *str, int s);
  void SendMsg2(char *str, int s);
  int RcvMsg(char *str);

  void Release(void);
  int len;

  int clientSock; /*client sock*/

  int connected;

};

#endif