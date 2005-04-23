#pragma once

#include "game_base.h"
#include "alife_space.h"
#include "script_export_space.h"
#include "client_id.h"

class CSE_Abstract;
class xrServer;
// [OLES] Policy:
// it_*		- means ordinal number of player
// id_*		- means player-id
// eid_*	- means entity-id
class GameEventQueue;

class	game_sv_GameState	: public game_GameState
{
	typedef game_GameState inherited;
protected:
	u32								m_RPointFreezeTime;
	xrServer*						m_server;
	GameEventQueue*					m_event_queue;
	BOOL							m_bVotingEnabled;
		
	//Events
	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );

	virtual		void				ReadOptions				(shared_str &options);
	virtual		void				ConsoleCommands_Create	();
	virtual		void				ConsoleCommands_Clear	();

public:
#define		TEAM_COUNT 4

	BOOL							sv_force_sync;
	float							rpoints_MinDist [TEAM_COUNT];
	xr_vector<RPoint>				rpoints	[TEAM_COUNT];
	
	// ������ ���� ������ ��������
	DEF_DEQUE(MAP_ROTATION_LIST,	xr_string);

	bool							m_bMapRotation;
	bool							m_bMapNeedRotation;
	bool							m_bFastRestart;
	MAP_ROTATION_LIST				m_pMapRotation_List;

	virtual		void				SaveMapList				();
/*	
	// scripts
	u64								m_qwStartProcessorTime;
	u64								m_qwStartGameTime;
	float							m_fTimeFactor;
*/
public:
	virtual		void				OnPlayerConnect			(ClientID id_who);
	virtual		void				OnPlayerDisconnect		(ClientID id_who, LPSTR Name, u16 GameID);
	virtual		void				OnPlayerReady			(ClientID id_who)							   {};
	virtual		void				OnPlayerEnteredGame		(ClientID id_who)	{};
	virtual		void				OnPlayerConnectFinished	(ClientID id_who)	{};
				void				GenerateGameMessage		(NET_Packet &P);

	virtual		void				OnRoundStart			();									// ����� ������
	virtual		void				OnRoundEnd				(LPCSTR reason);					// ����� ������

	virtual		void				MapRotation_AddMap		(LPCSTR MapName);
	virtual		bool				OnNextMap				()									{return false;}
	virtual		void				OnPrevMap				()									{}
	virtual		bool				SwitchToNextMap			()	{ return m_bMapNeedRotation; };
	
	virtual		BOOL				IsVoteEnabled			()	{return m_bVotingEnabled;};
	virtual		bool				IsVotingActive			()	{ return false; };
	virtual		void				SetVotingActive			( bool Active )	{ };
	virtual		void				OnVoteStart				(LPCSTR VoteCommand, ClientID sender)			{};
	virtual		void				OnVoteStop				()				{};

public:
									game_sv_GameState		();
	virtual							~game_sv_GameState		();
	// Main accessors
	virtual		game_PlayerState*	get_eid					(u16 id);
	virtual		game_PlayerState*	get_it					(u32 it);
	virtual		game_PlayerState*	get_id					(ClientID id);							
	virtual		LPCSTR				get_name_it				(u32 it);
	virtual		LPCSTR				get_name_id				(ClientID id);								
				LPCSTR				get_player_name_id		(ClientID id);								
	virtual		u16					get_id_2_eid			(ClientID id);
	virtual		ClientID			get_it_2_id				(u32 it);
	virtual		u32					get_players_count		();
	virtual		CSE_Abstract*		get_entity_from_eid		(u16 id);
				RPoint				getRP					(u16 team_idx, u32 rp_idx);
				u32					getRPcount				(u16 team_idx);
	// Signals
	virtual		void				signal_Syncronize		();
	virtual		void				assign_RP				(CSE_Abstract* E, game_PlayerState* ps_who);
	virtual		bool				IsPointFreezed			(RPoint* rp);
	virtual		void				SetPointFreezed			(RPoint* rp);
	
	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase);	
				CSE_Abstract*		spawn_begin				(LPCSTR N);
				CSE_Abstract*		spawn_end				(CSE_Abstract* E, ClientID id);

	// Utilities
	virtual		s32					get_option_i			(LPCSTR lst, LPCSTR name, s32 def = 0);
	virtual		string64&			get_option_s			(LPCSTR lst, LPCSTR name, LPCSTR def = 0);
	virtual		u32					get_alive_count			(u32 team);
	virtual		xr_vector<u16>*		get_children			(ClientID id_who);
	void							u_EventGen				(NET_Packet& P, u16 type, u16 dest	);
	void							u_EventSend				(NET_Packet& P);

	// Events
	virtual		void				OnCreate				(u16 id_who)					{}
	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_target)	= 0;			// TRUE=allow ownership, FALSE=denied
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_target)	= 0;			// TRUE=allow ownership, FALSE=denied
	virtual		void				OnDestroyObject			(u16 eid_who)							{};			


//	virtual		void				OnPlayerBuyFinished		(u32 id_who, NET_Packet& P)				{};



	virtual		void				OnHit					(u16 id_hitter, u16 id_hitted, NET_Packet& P);	//���-�� ������� Hit
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P){}; //����� ������� Hit

	// Main
	virtual		void				Create					(shared_str& options);
	virtual		void				Update					();
	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);				// full state
	virtual		void				net_Export_Update		(NET_Packet& P, ClientID id_to, ClientID id);		// just incremental update for specific client
	virtual		void				net_Export_GameTime		(NET_Packet& P);						// update GameTime only for remote clients
/*
	virtual		ALife::_TIME_ID		GetGameTime				();
	virtual		float				GetGameTimeFactor		();
	virtual		void				SetGameTime				(ALife::_TIME_ID GameTime);
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);
*/
	virtual		bool				change_level			(NET_Packet &net_packet, ClientID sender);
	virtual		void				save_game				(NET_Packet &net_packet, ClientID sender);
	virtual		bool				load_game				(NET_Packet &net_packet, ClientID sender);
	virtual		void				reload_game				(NET_Packet &net_packet, ClientID sender);
	virtual		void				switch_distance			(NET_Packet &net_packet, ClientID sender);

				void				AddDelayedEvent			(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );
				void				ProcessDelayedEvent		();
	virtual		BOOL				isFriendlyFireEnabled	()	{return FALSE;};
	virtual		BOOL				CanHaveFriendlyFire		()	= 0;
	virtual		void				teleport_object			(NET_Packet &packet, u16 id);
	virtual		void				add_restriction			(NET_Packet &packet, u16 id);
	virtual		void				remove_restriction		(NET_Packet &packet, u16 id);
	virtual		void				remove_all_restrictions	(NET_Packet &packet, u16 id);
	virtual		bool				custom_sls_default		() {return false;};
	virtual		void				sls_default				() {};
	virtual		shared_str			level_name				(const shared_str &server_options) const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

//---------------------------------------------------------------------------
#include "../xr_ioconsole.h"
#include "../xr_ioc_cmd.h"

#define CMD_ADD(cls,p1,p2,p3,p4,ctrl,p0)		{ if (!ctrl) {CMD4(cls,p1,(int*)p2,p3,p4); } \
												else {sprintf(p0,p1 " #set_%p", p2); Console->Execute(p0);}}
#define CMD_CLEAR(p1)							{Console->Execute(p1 " #set_0");}

class CCC_SV_Int;
class CCC_SV_Int : public CCC_Integer {
protected:
	int StoredValue;
public:
	CCC_SV_Int(LPCSTR N, int *pValue,int _min=0, int _max=999) :
	  CCC_Integer(N,pValue,_min,_max),
		  StoredValue(*pValue)
	  {};

	  virtual void	Execute	(LPCSTR args)
	  {
		  if (strstr(args, "#set_"))
		  {			  
			  int* pNewValue = NULL;
			  sscanf(args+5, "%x", &pNewValue);
			  if (NULL == pNewValue)
			  {
				  value = &StoredValue;
			  }
			  else
			  {
				  *pNewValue = StoredValue;
				  value = pNewValue;
			  };			  
			  return;
		  };

		  CCC_Integer::Execute(args);
		  StoredValue = *value;
	  }

	  virtual void	Save	(IWriter *F)	{};
	  virtual void	Status	(TStatus& S)
	  {
		  CCC_Integer::Status(S);
	  }
};
//---------------------------------------------------------------
add_to_type_list(game_sv_GameState)
#undef script_type_list
#define script_type_list save_type_list(game_sv_GameState)