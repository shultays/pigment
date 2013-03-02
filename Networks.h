#ifndef NETWORKS_H
#define NETWORKS_H

class Game;


#include "Tools.h"
#include "Client.h"
#include "NetworkStructs.h"
#include "Bullet.h"

#define GetLock() WaitForSingleObject(Networks::lock, INFINITE)
#define ReleaseLock() ReleaseMutex(Networks::lock)

class Networks{
public:

  static int pnum;
  static int num;
  static char clients[32];

  static Client_Init_struct clientsInit[32];
  static Client_State_struct clientsState[32];

  static Client_State_struct selfScene;

  static int recievedImpPackets[1024];
  static int recievedImpNum;

  static long WINAPI Thread(LPVOID lpParameter);

  static long WINAPI CheckImportantMessages(LPVOID lpParameter);


  static Client cli;
  static void Setup(char *ip, int port);
  static void Release(void);
  static Game *g;
  static int connected;

  static DWORD timediff;

  static HANDLE lock;

  static void getScene();
  static void sendScene();

  static void SendImpMsg(char *str, int s);
  static void SendStr(char *str);

  static void sendNewBulet(Bullet *b);
  static void sendBulletHit(int id, int hitType, int paintableNum, int index, float u, float v, D3DXVECTOR4 c);

  static void sendFlagDemand();
  static void sendFlagRelease(int type, D3DXVECTOR3 coor);

};


#endif