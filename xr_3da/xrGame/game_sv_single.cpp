#include "stdafx.h"
#include "game_sv_single.h"

void	game_sv_Single::Create			(LPCSTR options)
{
	switch_Phase	(GAME_PHASE_INPROGRESS);
}

BOOL	game_sv_Single::OnTouch			(u16 eid_who, u16 eid_what)
{
	return TRUE;
}

BOOL	game_sv_Single::OnDetouch		(u16 eid_who, u16 eid_what)
{
	return TRUE;
}
