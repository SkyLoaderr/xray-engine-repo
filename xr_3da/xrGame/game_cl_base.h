#pragma once

#include "game_base.h"

class	CGameObject;

class	game_cl_GameState	: public game_GameState
{
protected:
	ref_sound		pMessageSounds[8];

public:
	struct Player : public game_PlayerState 
	{
		string64	name;
	};
	xr_map<u32,Player>				players;
	u32								local_svdpnid;
	Player*							local_player;
	xr_vector<CGameObject*>			targets;
public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();

	virtual		void				Create					(LPSTR &options);
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_signal				(NET_Packet& P);

	virtual		void				OnGameMessage			(NET_Packet& P);

	virtual		game_cl_GameState::Player*				GetPlayerByGameID		(u32 GameID);

};
