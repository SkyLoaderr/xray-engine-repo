#pragma once

#include "game_base.h"

class	CGameObject;
class	CUIGameCustom;
class	CUI;

class	game_cl_GameState	: public game_GameState
{

public:
	struct Player : public game_PlayerState 
	{
		string64	name;
	};
	xr_map<u32,Player>				players;
	u32								local_svdpnid;
	Player*							local_player;
	xr_vector<CGameObject*>			targets;

protected:
	//for scripting enhancement
	CLASS_ID						getGameUICLASS_ID		(LPCSTR game_type_name);
	virtual		void				TranslateGameMessage	(u32 msg, NET_Packet& P);

public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();

	virtual		void				Create					(LPSTR &options);
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_signal				(NET_Packet& P);

				void				OnGameMessage			(NET_Packet& P);

	virtual		char*				getTeamSection			(int Team);

	virtual		game_cl_GameState::Player*					GetPlayerByGameID		(u32 GameID);
	virtual		LPCSTR				type_name				() const {return "base client game";};
				CUIGameCustom*		createGameUI			(CUI* parent);

};
