#pragma once

enum	GAME_TYPE
{
	GAME_UNKNOWN			= 0	,
	GAME_DEATHMATCH				,
	GAME_TEAMDEATHMATCH			,
	GAME_ARTEFACTHUNT			,

	GAME_END_LIST				,
};

static LPSTR	g_GameTypeName	[]		= {
	"UNKNOWN"					,
	"Deathmatch"				,
	"TeamDeathmatch"			,
	"ArtefactHunt"				,

	"END_LIST"					,
};