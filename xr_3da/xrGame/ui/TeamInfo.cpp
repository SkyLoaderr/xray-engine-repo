#include "StdAfx.h"
#include "TeamInfo.h"

u32			CTeamInfo::team1_color;
u32			CTeamInfo::team2_color;
shared_str	CTeamInfo::team1_name;
shared_str	CTeamInfo::team2_name;
Flags32		CTeamInfo::flags;

u32 CTeamInfo::GetTeam1_color(){
	if (flags.test(flTeam1_color))
		return team1_color;

	string256 _buff;

	LPCSTR tm_col = pSettings->r_string("team1","color");
	team1_color = color_argb(155,atoi(_GetItem(tm_col, 0, _buff)),atoi(_GetItem(tm_col, 1, _buff)),atoi(_GetItem(tm_col, 2, _buff)));
	flags.set(flTeam1_color, true);

	return team1_color;
}

u32 CTeamInfo::GetTeam2_color(){
	if (flags.test(flTeam2_color))
		return team2_color;

	string256 _buff;

	LPCSTR tm_col = pSettings->r_string("team2","color");
	team2_color = color_argb(155,atoi(_GetItem(tm_col, 0, _buff)),atoi(_GetItem(tm_col, 1, _buff)),atoi(_GetItem(tm_col, 2, _buff)));
	flags.set(flTeam2_color, true);

	return team2_color;
}

shared_str	CTeamInfo::GetTeam1_name(){
	if (flags.test(flTeam1_name))
		return team1_name;

	team1_name = pSettings->r_string_wb("team1","name");
	flags.set(flTeam1_name,true);

    return team1_name;
}

shared_str	CTeamInfo::GetTeam2_name(){
	if (flags.test(flTeam2_name))
		return team2_name;

	team2_name = pSettings->r_string_wb("team2","name");
	flags.set(flTeam2_name,true);

    return team2_name;
}