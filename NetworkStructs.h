#ifndef NETWORKSTRUCT_H
#define NETWORKSTRUCT_H

#pragma pack(push)
#pragma pack(1)   


enum TYPE {  MSG_FIN = 42, MSG_CLIENT_INIT, MSG_CLIENT_STATE, 
          MSG_SERVER_INIT, MSG_IMPORTANT, MSG_IMPPACKET_RECIEVED,
          MSG_TIME, MSG_STRING, MSG_NEW_BULLET, MSG_BULLET_HIT,
          MSG_FLAG_DEMAND, MSG_FLAG_RELEASE, MSG_FLAG_STATE
};


struct Client_Init_struct{
  TYPE t; 
  int num;
  char name[32];
  int model;  
  float coors[3];
  float pitch;
  float yaw;
  int side;
};

struct Client_State_struct{
  TYPE t;
  int num;
  unsigned int time;
  float coors[3];
  float speed[3];
  float pitch;
  float yaw;

  char anim_low;
  char anim_high;
  char sidewalk;
  char anim_low_reversed;
  char low_state;
  char high_state;
  char weapon;
  

  float life;
  char isDead;
  char side;
};

struct Server_Init_struct{
  TYPE t;
  int pnum;
  int i;
  char clients[32];
  unsigned int time;
};

struct Important_Packet_struct{
  TYPE t;
  int num;
  int packetnum;
  int psize;
  unsigned int lastsent;
};

struct ImpPacket_Recieved_struct{
  TYPE t;
  int num;
  int packetnum;
};

struct Time_struct{
  TYPE t;
  int time;
};

struct String_struct{
  TYPE t;
  int num;
  char str[64];
};

struct New_Bullet_struct{
  TYPE t;
  int num;
  float coors[3];
  float dir[3];
  float dieTime;
  float cpU, cpV;
  int cindex;
  int n;
  unsigned int initTime;
  int id;
  int side;
  float color[4];
};

struct Bullet_Hit_struct{
  TYPE t;
  int num;
  int id;
  int hitType;
  int index;
  float u, v;
  int paintableNum;
  float color[4];
};

struct Flag_Demand_struct{
  TYPE t;
  int num;
};

struct Flag_Release_struct{
  TYPE t;
  float coor[3];
  int type;

};

struct Flag_State_struct{
  TYPE t;
  int owner;
  float coor[3];
  float bar;
  int type;
  int side;
  unsigned int time;
};


#pragma pack(pop)

#endif