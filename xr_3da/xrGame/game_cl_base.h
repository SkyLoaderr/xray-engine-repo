#pragma once

#include "game_base.h"
#include "UIDynamicItem.h"


class	CGameObject;
class	CUIGameCustom;
class	CUI;
class	CUIDialogWnd;

struct SZoneMapEntityData{
	Fvector	pos;
	u32		color;
	SZoneMapEntityData(){pos.set(.0f,.0f,.0f);color = 0xff00ff00;}
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};
add_to_type_list(SZoneMapEntityData)
#undef script_type_list
#define script_type_list save_type_list(SZoneMapEntityData)


class	game_cl_GameState	: public game_GameState, public ISheduled
{
	typedef game_GameState	inherited;
	string256							m_game_type_name;
	CUIGameCustom*						m_game_ui_custom;
	bool								m_bCrosshair;	//был ли показан прицел-курсор HUD перед вызовом меню

public:
	typedef xr_map<ClientID,game_PlayerState*> PLAYERS_MAP;
	typedef PLAYERS_MAP::iterator PLAYERS_MAP_IT;

	PLAYERS_MAP							players;
	ClientID							local_svdpnid;
	game_PlayerState*					local_player;
	xr_vector<CGameObject*>				targets; //bases ???

private:
				void				switch_Phase			(u32 new_phase)		{inherited::switch_Phase(new_phase);};
protected:

	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase)	{};	

	//for scripting enhancement
	virtual		void				TranslateGameMessage	(u32 msg, NET_Packet& P);
				void				CommonMessageOut		(LPCSTR msg);

				void				StartMenu				(CUIDialogWnd* pDialog, bool bDoHideIndicators);
				void				StopMenu				(CUIDialogWnd* pDialog, bool bDoHideIndicators);

	virtual		float				shedule_Scale			();

				void				sv_GameEventGen			(NET_Packet& P);
				void				sv_EventSend			(NET_Packet& P);
public:
									game_cl_GameState		();
	virtual							~game_cl_GameState		();
				LPCSTR				type_name				() const {return m_game_type_name;};
				void				set_type_name			(LPCSTR s){strcpy(m_game_type_name,s);};
	virtual		void				Init					(){};
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_signal				(NET_Packet& P);

				bool				IR_OnKeyboardPress		(int dik);
				bool				IR_OnKeyboardRelease	(int dik);
				bool				IR_OnMouseMove			(int dx, int dy);


	virtual		bool				OnKeyboardPress			(int key){return false;};
	virtual		bool				OnKeyboardRelease		(int key){return false;};
				void				OnGameMessage			(NET_Packet& P);

	virtual		char*				getTeamSection			(int Team){return NULL;};

				game_PlayerState*	GetPlayerByGameID		(u32 GameID);
				game_PlayerState*	GetPlayerByOrderID		(u32 id);
				ClientID			GetClientIDByOrderID	(u32 id);
				u32					GetPlayersCount			() const {return players.size();};
	virtual		CUIGameCustom*		createGameUI			(){return NULL;};
	virtual		void				GetMapEntities			(xr_vector<SZoneMapEntityData>& dst)	{};


				void				StartStopMenu			(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	virtual		void				shedule_Update			(u32 dt);

	void							u_EventGen				(NET_Packet& P, u16 type, u16 dest);
	void							u_EventSend				(NET_Packet& P);

};
