#pragma once

#include "game_base.h"
#include "UIDynamicItem.h"


class	CGameObject;
class	CUIGameCustom;
class	CUI;

struct SZoneMapEntityData{
	Fvector	pos;
	u32		color;
};


class	game_cl_GameState	: public game_GameState, public ISheduled
{
	string256			m_game_type_name;
public:
	struct Player : public game_PlayerState 
	{
		string64	name;
	};
	xr_map<u32,Player>				players;
	u32								local_svdpnid;
	Player*							local_player;
	xr_vector<CGameObject*>			targets; //bases ???

protected:
	//for scripting enhancement
	virtual		void				TranslateGameMessage	(u32 msg, NET_Packet& P);
				void				CommonMessageOut		(LPCSTR msg);
public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();
				LPCSTR				type_name				() const {return m_game_type_name;};
				void				set_type_name			(LPCSTR s){strcpy(m_game_type_name,s);};
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_signal				(NET_Packet& P);

				void				OnGameMessage			(NET_Packet& P);

	virtual		char*				getTeamSection			(int Team);

	game_cl_GameState::Player*		GetPlayerByGameID		(u32 GameID);
	virtual		CUIGameCustom*		createGameUI			(){return NULL;};
	virtual		void				GetMapEntities(xr_vector<SZoneMapEntityData>& dst)	{};

	virtual float					shedule_Scale		();
	virtual void					shedule_Update		(u32 dt);

};
