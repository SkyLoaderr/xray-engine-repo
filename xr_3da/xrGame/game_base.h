#pragma once

#include "game_base_space.h"
#include "script_export_space.h"
#include "alife_space.h"

#pragma pack(push,1)

struct	game_PlayerState;//fw

struct		RPoint
{
	Fvector	P;
	Fvector A;
	RPoint(){P.set(.0f,0.f,.0f);A.set(.0f,0.f,.0f);}
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};
add_to_type_list(RPoint)
#undef script_type_list
#define script_type_list save_type_list(RPoint)

struct	game_PlayerState 
{
	string64	name;
	s16			team;
	s16			kills;
	s16			deaths;
	s32			money_total;
	s16			money_for_round;
	u16			flags;

	u16			ping;

	u16			GameID;

	//------Dedicated-------------------
	bool		Skip;
	//---------------------------
	u16			lasthitter;
	u16			lasthitweapon;
	u8			skin;
	//---------------------------
	u32			RespawnTime;
	//---------------------------
	s16			money_delta;
/*
private:
	game_PlayerState(const game_PlayerState&);
	void operator = (const game_PlayerState&);
*/
public:
					game_PlayerState		();
					~game_PlayerState		();
	virtual void	clear					();
			bool	testFlag				(u16 f);
			void	setFlag					(u16 f);
			void	resetFlag				(u16 f);
			LPCSTR	getName					(){return name;}
			void	setName					(LPCSTR s){strcpy(name,s);}

#ifndef AI_COMPILER
	virtual void	net_Export				(NET_Packet& P);
	virtual void	net_Import				(NET_Packet& P);
#endif
	//---------------------------------------

	DEF_VECTOR(PLAYER_ITEMS_LIST, u16);

	PLAYER_ITEMS_LIST	pItemList;

	s16					LastBuyAcount;
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};

add_to_type_list(game_PlayerState)
#undef script_type_list
#define script_type_list save_type_list(game_PlayerState)


struct	game_TeamState
{
	int			score;
	u16			num_targets;

	game_TeamState();
};


#pragma pack(pop)

class	game_GameState : public DLL_Pure
{
protected:
	s32								type;
	u16								phase;
	s32								round;
	u32								start_time;
//	u32								buy_time;
//	s32								fraglimit; //dm,tdm,ah
//	s32								timelimit; //dm
//	u32								damageblocklimit;//dm,tdm
//	xr_vector<game_TeamState>		teams;//dm,tdm,ah
	// for Artefact Hunt
//	u8								artefactsNum;//ah
//	u16								artefactBearerID;//ah,ZoneMap
//	u8								teamInPossession;//ah,ZoneMap
protected:
	virtual		void				switch_Phase			(u32 new_phase);
	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase)	{};	

public:
	game_GameState();
				u32					Type					() const						{return type;};
				u32					Phase					() const						{return phase;};
				s32					Round					() const						{return round;};
				s32					StartTime				() const						{return start_time;};
	virtual		void				Create					(ref_str& options)				{};
	virtual		LPCSTR				type_name				() const						{return "base game";};
//for scripting enhancement
	static		CLASS_ID			getCLASS_ID				(LPCSTR game_type_name, bool bServer);
	virtual		game_PlayerState*	createPlayerState()		{return xr_new<game_PlayerState>(); };

//moved from game_sv_base (time routines)
private:
	// scripts
	u64								m_qwStartProcessorTime;
	u64								m_qwStartGameTime;
	float							m_fTimeFactor;
public:
				void				SetGameTimeFactor		(ALife::_TIME_ID GameTime, const float fTimeFactor);
	virtual		ALife::_TIME_ID		GetGameTime				();
	virtual		float				GetGameTimeFactor		();
//	virtual		void				SetGameTime				(ALife::_TIME_ID GameTime);
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);


	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_GameState)
#undef script_type_list
#define script_type_list save_type_list(game_GameState)
