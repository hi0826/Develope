#pragma once 
enum CS_PROTOCOL
{
	CS_LOGIN = 1,
	CS_LOGOUT,
	CS_MOVE,
	CS_ATTACK,
	CS_SKILL
};

enum SC_PROTOCOL
{
	SC_LOG_SUCC = 1,
	SC_LOG_FAIL,
	SC_PUT_PLAYER,
	SC_PUT_NPC,
	SC_REMOVE_PLAYER,
	SC_REMOVE_NPC,
	SC_MOVE_PLAYER,
	SC_MOVE_NPC,
	SC_ATTACK_PLAYER,
	SC_HEATED_NPC
};

#pragma pack (push, 1)
#pragma region CLIENT_PROTOCOL
struct cs_packet_chat
{
	BYTE size;
	BYTE type;
	WCHAR message[256];
};

struct cs_packet_login /*서버와 Connet후 DB 연결위해*/
{
	BYTE size;
	BYTE type;
	WCHAR name[MAX_STR_SIZE];
};

struct cs_packet_logout /*LogOut 했다는 Packet*/
{
	BYTE size;
	BYTE type;
};

struct cs_packet_move  /*Move Packet*/
{
	BYTE     size;
	BYTE     type;
	WORD       id;
	XMFLOAT3  pos;
	WORD      dir;
	BYTE    state;
};

struct cs_packet_attack
{
	BYTE    size;
	BYTE    type;
	WORD      id;
};
#pragma endregion CLIENT_PROTOCOL






#pragma region  SERVER_PROTOCOL
struct sc_packet_logsucc
{
	BYTE    size;
	BYTE    type;
	WORD      id;
	XMFLOAT3 pos;
	WORD     dir;
	BYTE     lev;
	WORD     exp;
	WORD      hp;
	WORD     atk;
};

struct sc_packet_Logfail
{
	BYTE    size;
	BYTE    type;
};

struct sc_packet_put_player
{
	BYTE    size;
	BYTE    type;
	WORD      id;
	XMFLOAT3 pos;
	WORD     dir;
	BYTE   state;
};

struct sc_packet_put_npc
{
	BYTE    size;
	BYTE    type;
	WORD      id;
	BYTE   mtype;
	XMFLOAT3 pos;
	WORD     dir;
	BYTE   state;
};

struct sc_packet_move_player
{
	BYTE    size;
	BYTE    type;
	WORD      id;
	XMFLOAT3 pos;
	WORD     dir;
	BYTE   state;
};

struct sc_packet_move_npc
{
	BYTE    size;
	BYTE    type;
	WORD      id;
	XMFLOAT3 pos;
	WORD     dir;
	BYTE   state;
};

struct sc_packet_remove_player
{
	BYTE    size;
	BYTE    type;
	WORD      id;
};

struct sc_packet_remove_npc
{
	BYTE    size;
	BYTE    type;
	WORD      id;
};

struct sc_packet_attack_player
{
	BYTE size;
	BYTE type;
	WORD   id;
};

struct sc_packet_heated_npc
{
	BYTE size;
	BYTE type;
	WORD   id;
};
#pragma endregion  SERVER_PROTOCOL
#pragma pack (pop)