//----------------------------------------------------
// file: BuilderGame.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "LevelGameDef.h"

BOOL SceneBuilder::BuildGame()
{
	SExportStreams F;
    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt it = lst.begin();it!=lst.end();it++)
            (*it)->ExportGame(F);
    }

    // save spawn
    AnsiString lev_spawn = "level.spawn";
    m_LevelPath.Update	(lev_spawn);
    Engine.FS.MarkFile	(lev_spawn,true);
    if (F.spawn.chunk)
	    F.spawn.stream.SaveTo(lev_spawn.c_str(),0);

    // save game
	CFS_Memory GAME;
    GAME.write_chunk(WAY_PATROLPATH_CHUNK,	F.patrolpath.stream.pointer(),	F.patrolpath.stream.size());
    GAME.write_chunk(RPOINT_CHUNK,			F.rpoint.stream.pointer(),		F.rpoint.stream.size());
    GAME.write_chunk(AIPOINT_CHUNK,			F.aipoint.stream.pointer(),		F.aipoint.stream.size());
    
    AnsiString lev_game	= "level.game";
    m_LevelPath.Update	(lev_game);
    Engine.FS.MarkFile	(lev_game,true);
    if (GAME.size())
	    GAME.SaveTo		(lev_game.c_str(),0);

    return TRUE;
}

