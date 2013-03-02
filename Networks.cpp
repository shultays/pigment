#include "Networks.h"

#include "Game.h"
#include "DeadPlayer.h"

HANDLE Networks::lock = CreateMutex(NULL, false, NULL);
HANDLE importantMsgLock = CreateMutex(NULL, false, NULL);

#define GetImpLock() WaitForSingleObject(importantMsgLock, INFINITE)
#define ReleaseImpLock() ReleaseMutex(importantMsgLock)

Client Networks::cli;
Game* Networks::g = 0;

Client_Init_struct Networks::clientsInit[32];
Client_State_struct Networks::clientsState[32];
Client_State_struct Networks::selfScene;

struct Client_Init_struct *cis;
struct Client_State_struct *css;
struct Server_Init_struct *sis;
struct Important_Packet_struct *ips;
struct ImpPacket_Recieved_struct *irs;
struct Time_struct *ts;
struct String_struct *ss;
struct New_Bullet_struct *nbs;
struct Bullet_Hit_struct *bhs;
struct Flag_Demand_struct *fds;
struct Flag_Release_struct *frs;
struct Flag_State_struct *fss;

Flag_State_struct flag;

int Networks::connected = 0;

int Networks::pnum = 0;
int Networks::num = -1;
char Networks::clients[32] = {0};
DWORD Networks::timediff = 0;

int Networks::recievedImpPackets[1024] = {-1};
int Networks::recievedImpNum = 0;

char msg[4096];
char msgrc[4096];
char msg2[4096];

struct Important_Packet_struct impPackets[512];
int impsize = 0;
int imppacknum = 42;
char *impPacketDatas[512];

long WINAPI Networks::Thread(LPVOID lpParameter){

  int i;
  while(1){
    int z = cli.RcvMsg(msgrc);
    GetLock();
    int con = 1;
    char *c = msgrc;
    while(con){
      TYPE t = (TYPE)*c;
      switch (t){
  
        case MSG_SERVER_INIT:
          sis = (struct Server_Init_struct*)c;
          c += sizeof(struct Server_Init_struct);

          pnum = sis->pnum;

          num = sis->i;
          selfScene.num = sis->i;

          timediff = sis->time - g->CurrentTime;

          for(i=0; i<32; i++) clients[i] = sis->clients[i];
          
          connected = 1;


          break;
        case MSG_CLIENT_INIT:
          cis = (struct Client_Init_struct*)c;
          c += sizeof(struct Client_Init_struct);

          if(!clients[cis->num]){
            pnum++;
            clients[cis->num] = 1;
          }
          clientsInit[cis->num] = *cis;

          clientsState[cis->num].coors[0] = cis->coors[0];
          clientsState[cis->num].coors[1] = cis->coors[1];
          clientsState[cis->num].coors[2] = cis->coors[2];

          clientsState[cis->num].yaw  = cis->yaw;
          clientsState[cis->num].pitch  = cis->pitch;

          g->con.buff << "Player '";
          g->con.buff << cis->name;
          g->con.buff << "' connected.";
          g->con.Print();

          break;
        case MSG_CLIENT_STATE:
          css = (struct Client_State_struct*)c;
          c += sizeof(struct Client_State_struct);

          if(css->num < 0 || css->num > 32) ALERT1(css->num);
          if(css->time > clientsState[css->num].time)
            clientsState[css->num] = *css;


          break;
        case MSG_TIME:
          ts = (struct Time_struct*)c;
          c += sizeof(struct Time_struct);

          timediff = ts->time - g->CurrentTime;
          break;
        case MSG_IMPORTANT:
          ips = (struct Important_Packet_struct*) c;
          c += sizeof(struct Important_Packet_struct);

          irs = (struct ImpPacket_Recieved_struct*) msg2;
          irs->num = num;
          irs->packetnum = ips->packetnum;
          irs->t = MSG_IMPPACKET_RECIEVED;
        
          cli.SendMsg(msg2, sizeof(struct ImpPacket_Recieved_struct));
          
          for(i=0; i<1024; i++){
            if(recievedImpPackets[i] == ips->packetnum){
              //dup
              //g->con.Print("duplicate packet");
              con = 0;
              break;
            }
          }

          if(!con) break;
          //g->con.Print("recieved");
          recievedImpPackets[recievedImpNum] = ips->packetnum;
          recievedImpNum++;
          if(recievedImpNum==1024) recievedImpNum=0;
          
          break;

      case MSG_FLAG_STATE:
          fss = (struct Flag_State_struct*)c;
          c+= sizeof(struct Flag_State_struct);
          if(fss->time > flag.time){
            flag = *fss;
          }
          
          break;
      case MSG_IMPPACKET_RECIEVED:

          irs = (struct ImpPacket_Recieved_struct*)c;
          c+= sizeof(struct ImpPacket_Recieved_struct);

          for(i=0; i<impsize; i++){
            if(impPackets[i].packetnum == irs->packetnum) break;
          }

          if(i==impsize){
            break;
          }
          delete(impPacketDatas[i]);

          impsize--;
          impPackets[i] = impPackets[impsize];
          impPacketDatas[i] = impPacketDatas[impsize];
        
          break;

       case MSG_STRING:
          
          ss = (struct String_struct*) c;
          c += sizeof(struct String_struct);

          if(ss->num == -1){
            g->con.Print(ss->str);
          }else{
            g->con.buff << ss->num <<":"<<ss->str;
            g->con.Print();
          }

          break;

        
          
        case MSG_NEW_BULLET:          
          nbs = (struct New_Bullet_struct*) c;
          c += sizeof(struct New_Bullet_struct);

          nbs->initTime -= timediff;

          Bullet::add(nbs);

          break;

          
        case MSG_BULLET_HIT:

          bhs = (struct Bullet_Hit_struct*) c;
          c += sizeof(struct Bullet_Hit_struct);

          Bullet::gotHit(bhs->hitType, bhs->num, bhs->id, bhs->paintableNum, bhs->index, bhs->u, bhs->v, D3DXVECTOR4(bhs->color));
          break;

        case MSG_FIN:

        
          con = 0;
          break;  
		default:
          
		  Tools::g->con.Print("MALFORMED PACKET");
          con = 0;
          break;



      }

    }
    ReleaseLock();
  }

  return 0;
}


void Networks::SendImpMsg(char *str, int s){
  
  TYPE *t = (TYPE*)(str+s);
  *t = MSG_FIN;
  s += sizeof(TYPE);

  int s2 = s +  sizeof(struct Important_Packet_struct);


  impPackets[impsize].t =  MSG_IMPORTANT;
  impPackets[impsize].num = num;
  impPackets[impsize].packetnum = imppacknum;
  impPackets[impsize].psize = s2;
  impPackets[impsize].lastsent = g->CurrentTime;
  impPacketDatas[impsize] = (char*) malloc(sizeof(char)*s2);

  memcpy(impPacketDatas[impsize], &impPackets[impsize], sizeof(struct Important_Packet_struct));
  memcpy(impPacketDatas[impsize]+ sizeof(struct Important_Packet_struct), str, s);

  cli.SendMsg2(impPacketDatas[impsize], s2);


  impsize++;
  imppacknum++;
  
}


void Networks::Setup(char *ip, int port){

  flag.time = 0;

  for(int i=0; i<1024; i++)recievedImpPackets[i]=-1;
  recievedImpNum = 0;
  pnum = 0;
  num = 32;
  connected = 0;

  if(lock == NULL) ALERT;
  if(cli.Setup(ip, port)) return;

  char *m = msg;
  int l = 0;
  cis = (struct Client_Init_struct *)m;

  strcpy_s(cis->name, 32, g->name);
  cis->t = MSG_CLIENT_INIT;
  cis->model = 0;
  cis->num = -1;

  cis->coors[0] = g->ActiveCamera->Position[0];
  cis->coors[1] = g->ActiveCamera->Position[1];
  cis->coors[2] = g->ActiveCamera->Position[2];
  cis->pitch = g->ActiveCamera->Pitch;
  cis->yaw = g->ActiveCamera->Yaw;

  cis->side = g->side;

  l += sizeof(struct Client_Init_struct);

  //SendImpMsg(m, l);
  cli.SendMsg(m, l);

  selfScene.t = MSG_CLIENT_STATE;

  connected = -1;   
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, 0, 0, 0);
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckImportantMessages, 0, 0, 0);
}


void Networks::Release(){
  
  cli.Release(); 



  connected = 0;
}

void Networks::sendScene(void){

  if(connected != 1) return;
  //GetLock();
  selfScene.coors[0] = g->ActiveCamera->Position[0];
  selfScene.coors[1] = g->ActiveCamera->Position[1];
  selfScene.coors[2] = g->ActiveCamera->Position[2];

  selfScene.time = g->CurrentTime + timediff;

  selfScene.yaw = g->ActiveCamera->Yaw;
  selfScene.pitch = g->ActiveCamera->Pitch;

  selfScene.speed[0] = g->speedDir[0];
  selfScene.speed[1] = g->speedDir[1];
  selfScene.speed[2] = g->speedDir[2];

  selfScene.speed[1] = g->speed[1];

  selfScene.anim_low = (char)g->testplayer.anim_low;
  selfScene.anim_high = (char)g->testplayer.anim_high;

  selfScene.sidewalk = g->testplayer.sidewalk;
  selfScene.anim_low_reversed = g->testplayer.anim_low_reversed;
  selfScene.low_state = (char)g->testplayer.low_state;
  selfScene.high_state = (char)g->testplayer.high_state;

  selfScene.weapon = (char)g->weapon;

  selfScene.life = g->life;
  selfScene.isDead = g->isDead;
  selfScene.side = g->side;
  
  cli.SendMsg((char*)&selfScene, sizeof(Client_State_struct));

  //ReleaseLock();
}

#define showself 0

void Networks::getScene(){
  if(connected != 1) return;
  //GetLock();

  g->playerNum = pnum - 1 + showself;
  int j=0;
  D3DXVECTOR3 s;
  
  
  g->flag.owner = flag.owner;
  g->flag.percent = flag.bar;
  g->flag.side = flag.side;
  if(flag.owner == -1){
    g->flag.coors.x = flag.coor[0];
    g->flag.coors.y = flag.coor[1];
    g->flag.coors.z = flag.coor[2];
    g->flag.type = flag.type;
  }
  
  if(g->flag.owner == -1){
     g->flag.addDrawList();
     g->flag.recalculateWorld();
  }else{
     g->flag.removeDrawList();
  }

  if(g->flag.owner == num){
    g->testplayer.hasFlag = 1;
	
  }else{
    g->testplayer.hasFlag = 0;
  }

  for(int i=0; j<g->playerNum; i++){
    if(clients[i]){
      if(showself == 0 && i == num) continue;

      Player *current = &g->players[j];
      current->model = 0;

      current->side = clientsState[i].side;

      if(current->isDead == 0 && clientsState[i].isDead == 1){
        DeadPlayer *dp = new DeadPlayer();
        dp->addDeadList(current);
      }
      current->isDead = clientsState[i].isDead;

      if(current->anim_low != (ANIMATIONS_LOW)clientsState[i].anim_low){
        current->anim_low = (ANIMATIONS_LOW)clientsState[i].anim_low;
        current->framenumlow = 0;
      }

      if(current->anim_high != (ANIMATIONS_HIGH)clientsState[i].anim_high){
        current->anim_high = (ANIMATIONS_HIGH)clientsState[i].anim_high;
        current->framenumhigh = 0;
      }
      current->pitch = 0.8f*clientsState[i].pitch;
      current->yaw = -clientsState[i].yaw;
      current->sidewalk = clientsState[i].sidewalk;
      current->anim_low_reversed = clientsState[i].anim_low_reversed;

      current->low_state = (ANIMATIONS_LOW)clientsState[i].low_state;
      current->high_state = (ANIMATIONS_HIGH)clientsState[i].high_state;

      current->weapon = clientsState[i].weapon;

      current->life = clientsState[i].life;

      current->num = clientsState[i].num;

      if(g->flag.owner == current->num){
        current->hasFlag = 1;
      }else{
        current->hasFlag = 0;
      }

      float delta = (g->CurrentTime  - clientsState[i].time) * 0.001f;
      delta = 0;

      D3DXVECTOR3 nc;
      nc.x = clientsState[i].coors[0] + clientsState[i].speed[0]*delta;
      nc.y = clientsState[i].coors[1] + clientsState[i].speed[1]*delta;
      nc.z = clientsState[i].coors[2] + clientsState[i].speed[2]*delta;

      D3DXVECTOR3 diff = nc - current->coors;
      float d = D3DXVec3Length(&diff);

      if(d > 100.f){
      }else if(d > 10.0f) {
        diff *= 10.0f/d;
      } 

      current->coors += diff;
   

      j++;
    }
  }

  //ReleaseLock();
}

long WINAPI Networks::CheckImportantMessages(LPVOID lpParameter){
  while(1){
    GetLock();
    for(int i=0; i<impsize; i++){
      if(impPackets[i].lastsent - g->CurrentTime > 11){
        cli.SendMsg2(impPacketDatas[i], impPackets[i].psize);
        impPackets[i].lastsent = g->CurrentTime;
      }
    }
    ReleaseLock();
  
    Sleep(3);
  }
  return 0;
}

void Networks::SendStr(char str[512]){

 if(connected != 1) return;

 ss = (struct String_struct*)msg;

 ss->t = MSG_STRING;
 ss->num = num;
 memcpy(ss->str, str, 512);

 SendImpMsg(msg, sizeof(struct String_struct));

}


void Networks::sendNewBulet(Bullet *b){

 if(connected != 1) return;

 nbs = (struct New_Bullet_struct*)msg;
 nbs->t = MSG_NEW_BULLET;
 nbs->num = num;
 nbs->coors[0] = b->coors[0];
 nbs->coors[1] = b->coors[1];
 nbs->coors[2] = b->coors[2];
 
 nbs->side = b->side;
 
 nbs->dir[0] = b->dir[0];
 nbs->dir[1] = b->dir[1];
 nbs->dir[2] = b->dir[2];

 nbs->cpU = b->cpU;
 nbs->cpV = b->cpV;
 nbs->dieTime = b->dieTime;
 nbs->n = b->n;


 nbs->color[0] = b->color.x;
 nbs->color[1] = b->color.y;
 nbs->color[2] = b->color.z;
 nbs->color[3] = b->color.w;

 nbs->cindex = b->cindex;

 nbs->initTime = b->initTime + timediff;

 nbs->id = b->id;

 SendImpMsg(msg, sizeof(struct New_Bullet_struct));

}


void Networks::sendBulletHit(int id, int hitType, int paintableNum, int index, float u, float v, D3DXVECTOR4 c){       
  if(connected != 1) return;

  bhs = (struct Bullet_Hit_struct*)msg;
  bhs->t = MSG_BULLET_HIT;
  bhs->id = id;
  bhs->num = num;
  bhs->hitType = hitType;
  bhs->index = index;
  bhs->u = u;
  bhs->v = v;
  bhs->paintableNum = paintableNum;
  bhs->color[0] = c.x;
  bhs->color[1] = c.y;
  bhs->color[2] = c.z;
  bhs->color[3] = c.w;


  SendImpMsg(msg, sizeof(struct Bullet_Hit_struct)); 
}

void Networks::sendFlagDemand(){
  if(connected != 1) return;

  fds = (struct Flag_Demand_struct*)msg;
  fds->num = num;
  fds->t = MSG_FLAG_DEMAND;

  cli.SendMsg(msg, sizeof(struct Flag_Demand_struct));

}

void Networks::sendFlagRelease(int type, D3DXVECTOR3 coor){
  if(connected != 1) return;

  frs = (struct Flag_Release_struct*)msg;
  frs->t = MSG_FLAG_RELEASE;
  frs->type = type;
  frs->coor[0] = coor.x;
  frs->coor[1] = coor.y;
  frs->coor[2] = coor.z;

  SendImpMsg(msg, sizeof(struct Flag_Release_struct));

}