#include "stdafx.h"
#include "game_base.h"
#include "ai_space.h"
#include "script_engine.h"
#include "script_space.h"

game_PlayerState::game_PlayerState()
{
	team				=	-1;
	kills				=	0;
	deaths				=	0;
	money_total			=	0;
	money_for_round		=	0;
	flags				=	0;

	skin				=	0;

	Skip = false;
	//---------------------------------
	pItemList.clear();

	LastBuyAcount = 0;
}

game_PlayerState::~game_PlayerState()
{
	pItemList.clear();
	LastBuyAcount = 0;
};

void	game_PlayerState::net_Export		(NET_Packet& P)
{
	P.w_s16			(	team	);
	P.w_s16			(	kills	);
	P.w_s16			(	deaths	);
	P.w_s32			(	money_total	);
	P.w_s16			(	money_for_round	);
	P.w_u16			(	flags	);
	P.w_u16			(	ping	);

	P.w_u16			(	GameID	);
};

void	game_PlayerState::net_Import		(NET_Packet& P)
{
	P.r_s16			(	team	);
	P.r_s16			(	kills	);
	P.r_s16			(	deaths	);
	P.r_s32			(	money_total	);
	P.r_s16			(	money_for_round	);
	P.r_u16			(	flags	);
	P.r_u16			(	ping	);

	P.r_u16			(	GameID	);
};

game_TeamState::game_TeamState()
{
	score				=	0;
	num_targets			=	0;
}

game_GameState::game_GameState()
{
	type				=	-1;
	phase				=	GAME_PHASE_PENDING;
	round				=	-1;
//	fraglimit			=	-1;
//	timelimit			=	-1;
//	buy_time			=	60000; // 60 sec
}

CLASS_ID game_GameState::getCLASS_ID(LPCSTR game_type_name, bool isServer)
{
	string256		S;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);

	string256				I;
	strcpy(I,l_tpIniFile->r_string("common","game_type_clsid_factory"));

	luabind::functor<LPCSTR>	result;
	R_ASSERT					(ai().script_engine().functor(I,result));
	ref_str clsid = result		(game_type_name, isServer);

	xr_delete			(l_tpIniFile);
	
	return TEXT2CLSID(*clsid);
}
