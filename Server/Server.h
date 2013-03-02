
#include <stdio.h>
#include <winsock2.h>
#include <mmsystem.h>
#include <Windows.h>


#include "../NetworkStructs.h"

#define packetloss 0

int fin;

char msg[4096];
char msgSend[4096];

int l;

struct Flag_State_struct flag;

struct Important_Packet_struct impPackets[1024];
int impsize = 0;
int imppacknum = 0;
char *impPacketDatas[1024];

SOCKET servsocket;

sockaddr_in SenderAddr;
int SenderAddrSize = sizeof(SenderAddr);

TYPE EoM = MSG_FIN;

char clients[32];
char clientsP[32];
char clientsI[32];
sockaddr_in csockets[32];

struct Client_Init_struct clientsInit[32];
struct Client_State_struct clientsState[32];

int pnum;

DWORD CurrentTime;
DWORD LastTime;
float DeltaTime;


struct Client_Init_struct *cis;
struct Client_State_struct *css;
struct ImpPacket_Recieved_struct *irs;
struct Important_Packet_struct *ips;
struct String_struct *ss;
struct New_Bullet_struct *nbs;
struct Bullet_Hit_struct *bhs;
struct Flag_Demand_struct *fds;
struct Flag_Release_struct *frs;


int recievedImpPackets[32][1024] = {-1};
int recievedImpNum[32] = {0};


DWORD lastSceneSent;
DWORD timeSent;

DWORD lastSent[10];
DWORD intervals[10];
void (*functions[10])();
int fnum = 2;

void UpdateDeltaTime() {
  CurrentTime = timeGetTime();
  DeltaTime = ((float)CurrentTime - (float)LastTime) * 0.001f;
  LastTime = CurrentTime;
}


void sendImpMsg(int i, int s, char *str){
  TYPE *t = (TYPE*)(str+s);
  *t = MSG_FIN;
  s += sizeof(TYPE);


  int s2 = s +  sizeof(struct Important_Packet_struct);

  impPackets[impsize].t =  MSG_IMPORTANT;
  impPackets[impsize].num = i;
  impPackets[impsize].packetnum = imppacknum;
  impPackets[impsize].psize = s2;
  impPackets[impsize].lastsent = CurrentTime;
  impPacketDatas[impsize] = (char*) malloc(sizeof(char)*s2);

  memcpy(impPacketDatas[impsize], &impPackets[impsize], sizeof(struct Important_Packet_struct));
  memcpy(impPacketDatas[impsize]+ sizeof(struct Important_Packet_struct), str, s);

  sendto(servsocket, impPacketDatas[impsize], s2, 0, (SOCKADDR *)&csockets[i],sizeof(csockets[i]));
 
  impsize++;
  imppacknum++;
}

int first = 10;
void sendMsg(int i, int s, char *str){
#if packetloss > 0
  if(rand()%packetloss != 0 && first-- < 0) return;
#endif
  TYPE *t = (TYPE*)(str+s);
  *t = MSG_FIN;
  s += sizeof(TYPE);
 
  sendto(servsocket, str, s, 0, (SOCKADDR *)&csockets[i],sizeof(csockets[i]));
}


void sendInit(int i){

  char *c = msgSend;
  
  int l = 0;
  struct Server_Init_struct * sis = (struct Server_Init_struct *)c;
  sis->t = MSG_SERVER_INIT;
  sis->pnum = pnum;
  sis->i = i;
  sis->time = CurrentTime;
  for(int k=0; k<32; k++) sis->clients[k] = clients[k];

  l += sizeof(Server_Init_struct);

  for(int k=0; k<pnum; k++){
    clientsInit[k].coors[0] = clientsState[k].coors[0];
    clientsInit[k].coors[1] = clientsState[k].coors[1];
    clientsInit[k].coors[2] = clientsState[k].coors[2];
    
    clientsInit[k].pitch = clientsState[k].pitch;
    clientsInit[k].yaw = clientsState[k].yaw;
  }

  memcpy(c+l, &clientsInit, sizeof(struct Client_Init_struct)*pnum);

  l+= sizeof(struct Client_Init_struct)*pnum;
  
  sendImpMsg(i, l, msgSend);


  ss = (String_struct*) msgSend;
  ss->t = MSG_STRING;
  ss->num = -1;
  strcpy(ss->str, "Hello from Server!");
  sendImpMsg(i, sizeof(String_struct), msgSend);
}

void dispatchMessage(){
  int con = 1;
  int i;
  int t1;
  int pn;
  char *c = msg;
  do{
    TYPE t = (TYPE)*c;
    switch (t){
        case MSG_CLIENT_INIT:
          cis = (struct Client_Init_struct*)c;
          c+= sizeof(struct Client_Init_struct);
          
          i=0;
          if(cis->num == -1){
            for(i=0; i<32; i++){
              if(clients[i]){
                if(strcmp(inet_ntoa(csockets[i].sin_addr), inet_ntoa(SenderAddr.sin_addr)) && csockets[i].sin_port == SenderAddr.sin_port){
                  //printf("double\n");
                  break;
                }
              }
            }
            for(i=0; i<32; i++){
              if(clients[i] == 0) break;
            }
            csockets[i] = SenderAddr;
            clientsP[i] = pnum;
            clientsI[pnum] = i;
            clients[i] = 1;
            pnum++;
          }else i = cis->num;

          pn = clientsP[i];
          clientsInit[pn] = *cis;

          sprintf(clientsInit[pn].name+strlen(clientsInit[pn].name), " %d", i);

          clientsInit[pn].num = i;

          clientsState[pn].time = CurrentTime;
          clientsState[pn].coors[0] = cis->coors[0];
          clientsState[pn].coors[1] = cis->coors[1];
          clientsState[pn].coors[2] = cis->coors[2];
          clientsState[pn].yaw = cis->yaw;
          clientsState[pn].pitch = cis->pitch;

          clientsState[pn].speed[0] = 0;
          clientsState[pn].speed[1] = 0;
          clientsState[pn].speed[2] = 0;

          if(cis->num == -1) sendInit(i);
          cis->num=i;
          memcpy(msgSend,cis,sizeof(struct Client_Init_struct));
          t1=0;
          for(i=0; t1<pnum; i++){
            if(clients[i]){
              t1++;
              if(i != cis->num) sendImpMsg(i, sizeof(struct Client_Init_struct), msgSend);
            }
          }
          break;
        
        case MSG_CLIENT_STATE:
          css = (struct Client_State_struct*)c;
          c+= sizeof(struct Client_State_struct);

          pn = clientsP[css->num];
          if(css->time > clientsState[pn].time)
            clientsState[pn] = *css;

          break;
        case MSG_IMPPACKET_RECIEVED:
          irs = (struct ImpPacket_Recieved_struct*)c;
          c+= sizeof(struct ImpPacket_Recieved_struct);
          //printf("%d sent\n", irs->packetnum);
          for(i=0; i<impsize; i++){
            if(impPackets[i].num == irs->num && impPackets[i].packetnum == irs->packetnum) break;
          }
          if(i==impsize) break;
          delete(impPacketDatas[i]);
          impsize--;
          impPackets[i] = impPackets[impsize];
          impPacketDatas[i] = impPacketDatas[impsize];
          break;

        case MSG_IMPORTANT:
          ips = (struct Important_Packet_struct*) c;
          c += sizeof(struct Important_Packet_struct);

          irs = (struct ImpPacket_Recieved_struct*) msgSend;
          irs->num = -1;
          irs->packetnum = ips->packetnum;
          irs->t = MSG_IMPPACKET_RECIEVED;
        
          sendMsg(clientsI[ips->num], sizeof(struct ImpPacket_Recieved_struct), (char*)irs);

          for(int i=0; i<1024; i++){
            if(recievedImpPackets[ips->num][i] == ips->packetnum){
              //dup
              printf("duplicate packet");
              con = 0;
              break;
            }
          }

          if(!con) break;
          
          recievedImpPackets[ips->num][recievedImpNum[ips->num]] = ips->packetnum;
          recievedImpNum[ips->num]++;
          if(recievedImpNum[ips->num]==1024) recievedImpNum[ips->num]=0;
          break;

        case MSG_STRING:
          
          ss = (struct String_struct*) c;
          c += sizeof(struct String_struct);

          printf("%d:%s\n", ss->num, ss->str);

          for(int i=0; i<pnum; i++){
            if(i==ss->num)continue;
            sendImpMsg(clientsI[i], sizeof(struct String_struct), (char*)ss);
          }


          break;
          
        case MSG_NEW_BULLET:

          
          nbs = (struct New_Bullet_struct*) c;
          c += sizeof(struct New_Bullet_struct);


          for(int i=0; i<pnum; i++){
            if(i==nbs->num)continue;
            sendImpMsg(clientsI[i], sizeof(struct New_Bullet_struct), (char*)nbs);
          }

          break;
      case MSG_BULLET_HIT:

          
          bhs = (struct Bullet_Hit_struct*) c;
          c += sizeof(struct Bullet_Hit_struct);

          //printf("%d\n", bhs->id);

          for(int i=0; i<pnum; i++){
            if(i==bhs->num)continue;
            sendImpMsg(clientsI[i], sizeof(struct Bullet_Hit_struct), (char*)bhs);
          }

          break;
      case MSG_FLAG_DEMAND:
        
          
        fds = (struct Flag_Demand_struct*) c;
        c += sizeof(struct  Flag_Demand_struct);

        printf("%d %d\n", flag.owner, fds->num);
        if(flag.owner == -1){
          flag.owner = fds->num;
          flag.type = -1;
        }
        

        break;
      case MSG_FLAG_RELEASE:
        
          
        frs = (struct Flag_Release_struct*) c;
        c += sizeof(struct  Flag_Release_struct);

        flag.owner = -1;
        flag.coor[0] = frs->coor[0];
        flag.coor[1] = frs->coor[1];
        flag.coor[2] = frs->coor[2];
        flag.type = frs->type;


        break;
      
      case MSG_FIN:

        
          con = 0;
          break;

        default:
          printf("err %d\n", t);
          con = 0;
          break;

      }

    }while(con);

}

void sendScene(){
  for(int i=0; i<pnum; i++){
    if(clientsState[i].num > 32 || clientsState[i].num < 0) printf("%d\n", clientsState[i].num);
  }
  float timediff = (CurrentTime - flag.time)*0.001f;
  timediff /= 100;
  flag.time = CurrentTime;

  
  if(flag.type == 0){
    if(flag.side == 1 && flag.bar > 0){
      flag.bar -= timediff*2;
    }else{
      flag.side = 0;
      flag.bar += timediff;
    }
  }else if(flag.type == 1){
    if(flag.side == 0 && flag.bar > 0){
      flag.bar -= timediff*2;
    }else{
      flag.side = 1;
      flag.bar += timediff;
    }
  }else if(flag.bar > 0){
    flag.bar -= timediff;
  }

  if(flag.bar >= 1.0){
	  flag.bar = 0;
	  flag.coor[0] = 0;
	  flag.coor[1] = 2720;
	  flag.coor[2] = 0;
	  flag.type = 2;
	  flag.side = -1;
	  flag.t = MSG_FLAG_STATE;
	  flag.owner = -1;
  }


  memcpy(msgSend, &flag, sizeof(flag));

  for(int i=0; i<pnum; i++){
    sendMsg(clientsI[i], sizeof(struct Client_State_struct)*pnum, (char*)clientsState);
    sendMsg(clientsI[i], sizeof(flag), msgSend);
  }
}

void sendTime(){
  
  struct Time_struct *ts = (struct Time_struct*)msgSend ;
  ts->t = MSG_TIME;

  ts->time = CurrentTime;
  for(int i=0; i<pnum; i++){
    sendMsg(clientsI[i], sizeof(struct Time_struct), msgSend);
  }
}

void checkImportantMessages(){
  for(int i=0; i<impsize; i++){
    if(impPackets[i].lastsent - CurrentTime > 11){
      sendto(servsocket, impPacketDatas[i], impPackets[i].psize, 0, (SOCKADDR *)&csockets[impPackets[i].num],sizeof(csockets[impPackets[i].num])); 
      impPackets[i].lastsent = CurrentTime;
    }
  }
}

void checkNewMessages(){ 
  while(1){
    int n = recvfrom(servsocket, msg, 4096, 0, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
    if(n > 0){
      dispatchMessage();
    }else break;
  }
}

long WINAPI ServerLoop(LPVOID lpParameter){
  while(!fin){
    UpdateDeltaTime();

      for(int i=0; i<fnum; i++){
        if(lastSent[i] < CurrentTime - intervals[i]){
        lastSent[i] = CurrentTime;
        (*functions[i])();
      }
    }


    Sleep(1);
  }
  return 0;
}

void StartServer(SOCKET s_socket){
  UpdateDeltaTime();

  fin = 0;

  flag.bar = 0;
  flag.coor[0] = 0;
  flag.coor[1] = 2720;
  flag.coor[2] = 0;
  flag.type = 2;
  flag.side = -1;
  flag.t = MSG_FLAG_STATE;
  flag.owner = -1;


  servsocket = s_socket;

  functions[0] = checkNewMessages;
  intervals[0] = 0;
  functions[1] = sendScene;
  intervals[1] = 11;
  functions[2] = checkImportantMessages;
  intervals[2] = 2;
  functions[3] = sendTime;
  intervals[3] = 5003;
  fnum = 4;

  for(int i=0; i<fnum; i++) lastSent[i] = CurrentTime;

  pnum = 0;
  for(int i=0; i<32; i++){
    clients[i] = 0;
    clientsInit[i].t = MSG_CLIENT_INIT;
    clientsState[i].t = MSG_CLIENT_STATE;
  }
  
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerLoop, 0, 0, 0);
  char line[128];

  while(1){
    fgets(line, 128, stdin);

    if(strcmp(line, "bye\n") == 0 || strcmp(line, "fin\n") == 0 ){
      printf("bye\n.");
      fin = 1;
      break;
    }
  }
}
