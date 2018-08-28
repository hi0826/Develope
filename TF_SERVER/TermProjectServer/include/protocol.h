#pragma once 
enum CS_PROTOCOL
{
	CS_LOGIN = 1,
	CS_LOGOUT,
	CS_MOVE,
	CS_ATTACK,
	CS_COLLISION,
	CS_CHANGE_CHARACTER,
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
	SC_STATE_NPC,
	SC_CLALI_NPC,
	SC_ATTACK_PLAYER,
	SC_HEATED_NPC,
	SC_CHAGNE_CHARACTER,
	SC_COLLISION_PLAYER,
	SC_DEAD_PLAYER,
	SC_DAMAGESTEP_PLAYER
};

#pragma pack (push, 1)
#pragma region CLIENT_PROTOCOL
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
struct cs_packet_collision
{
	BYTE    size;
	BYTE    type;
	WORD      id;
	XMFLOAT3 pos;
};
struct cs_packet_change_character
{
	BYTE  size;
	BYTE  type;
	WORD    id;
	BYTE   num;
};

#pragma endregion CLIENT_PROTOCOL

#pragma region  SERVER_PROTOCOL
struct sc_packet_logsucc
{
	BYTE        size;
	BYTE        type;
	WORD          id;
	XMFLOAT3     pos;
	WORD         dir;
	BYTE       state;
	BYTE   character;
	BYTE       stage;
	float         hp;
	float         mp;
	float        atk;
};

struct sc_packet_Logfail
{
	BYTE    size;
	BYTE    type;
};

struct sc_packet_put_player
{
	BYTE        size;
	BYTE        type;
	WORD          id;
	XMFLOAT3     pos;
	WORD         dir;
	BYTE       state;
	BYTE   character;
};

struct sc_packet_put_npc
{
	BYTE        size;
	BYTE        type;
	WORD          id;
	BYTE       mtype;
	XMFLOAT3     pos;
	WORD         dir;
	BYTE       state;
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

struct sc_packet_state_npc
{
	BYTE        size;
	BYTE        type;
	WORD          id;
	WORD         dir;
	XMFLOAT3     pos;
	BYTE       state;
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

struct sc_packet_dead_player
{
	BYTE    size;
	BYTE    type;
	WORD      id;
};

struct sc_packet_heated_npc
{
	BYTE size;
	BYTE type;
	WORD   id;
};

struct sc_packet_collision_player
{
	BYTE    size;
	BYTE    type;
	WORD      id;
	XMFLOAT3 pos;
};

struct sc_packet_change_character
{
	BYTE  size;
	BYTE  type;
	WORD    id;
	BYTE   num;
};
struct sc_packet_damagestep_player
{
	BYTE   size;
	BYTE   type;
	WORD     id;
	float    hp;
};

#pragma endregion  SERVER_PROTOCOL
#pragma pack (pop)