#pragma once
enum SCENE_CREATE
{
	SC_CURRENT,
	SC_NEXT
};

enum  READ_DATA
{
	TREE = 1,
	Monster
};

/*ANIMSTATE*/
static const BYTE IDLESTATE   = 0;
static const BYTE MOVESTATE   = 1;
static const BYTE ATTACKSTATE = 2;
static const BYTE HEATSTATE   = 3;
static const BYTE DEADSTATE   = 4;



/*DIRECTION*/
#define DIR_FORWARD   0x01
#define DIR_BACKWARD  0x02
#define DIR_LEFT      0x04
#define DIR_RIGHT     0x08
#define DIR_FR        0x10
#define DIR_FL        0x20
#define DIR_BR        0x40
#define DIR_BL        0x80

