#pragma once
#define EPSILON		     1.0e-10f
#define MAX_BUFF_SIZE    4000
#define MAX_PACKET_SIZE  255
#define MAX_STR_SIZE     50


#define VIEW_RADIUS      30      
#define TRACE_RADIUS     20
#define ATTACK_RADIUS    5

#define MAX_USER		 1000      
#define NPC_START        1000    
#define NPC_END          1500
#define NUM_OF_NPC       500  
#define MY_SERVER_PORT   9000


static const BYTE IDLESTATE = 0;
static const BYTE MOVESTATE = 1;
static const BYTE ATTACKSTATE = 2;
static const BYTE HEATSTATE = 3;
static const BYTE DEADSTATE = 4;

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
	NPC_MOVE,
	NPC_ATTACK,
	NPC_TRACE,
	NPC_RESPAWN,
};

/*MONSTER_TYPE*/
enum MONSTER_TYPE
{
	STONEMON,
	BEATLEMON,
};




