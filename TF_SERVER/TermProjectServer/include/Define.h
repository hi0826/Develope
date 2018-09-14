#pragma once
#define EPSILON		     1.0e-10f
#define MAX_BUFF_SIZE    4000
#define MAX_PACKET_SIZE  255
#define MAX_STR_SIZE     50


#define VIEW_RADIUS      50      
#define TRACE_RADIUS     20
#define ATTACK_RADIUS    8
#define NONE_TARGET      9999

#define MAX_USER		 1000      
#define NPC_START        1000    
#define NUM_OF_NPC       1208
#define MY_SERVER_PORT   9000

static const BYTE IDLESTATE   = 0;
static const BYTE MOVESTATE   = 1;
static const BYTE ATTACKSTATE = 2;
static const BYTE HEATSTATE   = 3;
static const BYTE DEADSTATE   = 4;




static const int NUM_OF_DIRECTION = 8;

#define DIR_FORWARD      0x01
#define DIR_BACKWARD     0x02
#define DIR_LEFT         0x04
#define DIR_RIGHT        0x08
#define DIR_FR           0x10
#define DIR_FL           0x20
#define DIR_BR           0x40
#define DIR_BL           0x80


//IO_TYPE
enum IO_TYPE
{
	IO_RECV,
	IO_SEND,
	IO_EVENT
}; 

enum EV_TYPE
{   
	NPC_IDLE,
	NPC_MOVE,
	NPC_ATTACK,
	NPC_HEATED,
	NPC_DEAD,
	PLAYER_RESPAWN
};

enum MONSTER_TYPE
{
	STONEMON = 2,
	BEATLEMON,
	STONEBOSS,
	BEATLEBOSS
};

enum CHARACTER
{
	WARRIOR,
	PAIRY,
	GOAT
};

enum STAGE
{
	STAGE1,
	STAGE2,
	STAGE3
};

enum  READ_DATA
{
	TREE = 1,
	SMON,
	BMON,
	SMONBOSS,
	BMONBOSS
};

#define SAFE_DELETE(p) if(p)  {delete p; p =NULL;}
#define SAFE_DELETE_ARRAY(p) if(p) {delete[] p ; p = NULL;}
#define SAFE_RELEASE(p) if(p) { p->Release();  p =NULL;}

template<typename T>
void Safe_Delete_VecList(T& p)
{
	for (auto& iter : p)
		delete &iter;
		//SAFE_DELETE(iter);
	p.clear();
}

template<typename T>
void Safe_Delete_Map(T& p)
{
	for (auto& iter : p)
		SAFE_DELETE(iter->second);
	p.clear();
}