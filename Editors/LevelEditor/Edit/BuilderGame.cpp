//----------------------------------------------------
// file: BuilderGame.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "WayPoint.h"
#include "RPoint.h"
#include "Event.h"
#include "net_utils.h"
#include "xrMessages.h"

bool SceneBuilder::BuildGame()
{
    CFS_Memory SPAWN;
	int chunk = 0;
    // add event
    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            if ((*_F)->ExportSpawn(SPAWN,chunk)) chunk++;
    }
    if (chunk){
	    AnsiString lev_spawn = "level.spawn";
    	m_LevelPath.Update	(lev_spawn);
	    SPAWN.SaveTo		(lev_spawn.c_str(),0);
    }

// game
	CFS_Memory GAME;
	// way points
	if (Scene.ObjCount(OBJCLASS_WAY)) {
		ObjectIt _F  = Scene.FirstObj(OBJCLASS_WAY);
        ObjectIt _E  = Scene.LastObj(OBJCLASS_WAY);
        for (EWayType t=0; t<wtMaxType; t++){
        	GAME.open_chunk(t+0x1000);
        	chunk=0;
	        for(ObjectIt it=_F; it!=_E; it++){
    	    	CWayObject* P = (CWayObject*)(*it);
                if (P->GetType()==t){
                    if (P->ExportGame(GAME,chunk)) chunk++;
                }
			}
        	GAME.close_chunk();
        }
    }
    if (GAME.size()){
	    AnsiString lev_game	= "level.game";
    	m_LevelPath.Update	(lev_game);
	    GAME.SaveTo			(lev_game.c_str(),0);
    }

    return true;
}

