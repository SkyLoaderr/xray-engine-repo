#include "stdafx.h"
#include "game_sv_single.h"

void	game_sv_Single::Create			(LPCSTR options)
{
	phase	= GAME_PHASE_INPROGRESS;
}

BOOL	game_sv_Single::OnTargetTouched(u32 id_who, u32 id_target)
{
	return TRUE;
}
