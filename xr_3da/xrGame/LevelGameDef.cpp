#include "stdafx.h"
#pragma hdrstop

#include "LevelGameDef.h"

xr_token rpoint_type[]={
	{ "Actor Spawn",	rptActorSpawn	},
	{ "Artefact Spawn",	rptArtefactSpawn	},
	{ 0,				0	}
};

xr_token rpoint_gametype[]={
	{ "Any game",		rptGameAny				},
	{ "Deathmatch",		rptGameDeathmatch		},
	{ "TeamDeathmatch",	rptGameTeamDeathmatch	},
	{ "ArtefactHunt",	rptGameArtefactHunt		},
	{ 0,				0	}
};
 