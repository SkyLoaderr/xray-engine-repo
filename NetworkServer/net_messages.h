#pragma once

// messages
enum ENGINE_API ENetMessage {
	MSG_NONE = 0,
	SYS_CONNECT,
	SYS_DISCONNECT,

	M_CHAT,
	M_ACTOR_UPDATE,
	M_ACTOR_WEAPON_CHANGE,
	M_ACTOR_FIRE_START,	// Fvector P,D - interpolated as relative to Update's P,D
	M_ACTOR_FIRE_STOP,
	M_ACTOR_HIT_ANOTHER,// dpID, int power
	M_ACTOR_RESPAWN,	// Fvector
	M_ACTOR_TAKE_ITEM,	// object CID

	net_last,
	net_forcedword = DWORD(-1)
};

// registered messages
struct ENGINE_API	MSG_DEF
{
	DWORD	dwType;
	char*	pName;
	BOOL	bBroadcast;
};

extern 	void	net_RegisterMessages	();
extern 	void	net_LogMessage			(char *op, DWORD dwType);
extern 			vector<MSG_DEF*>		net_messages;
