#include "stdafx.h"
#include "HUDmanager.h"
#include "WayPointDef.h"

BOOL CLevel::Load_GameSpecific_Before()
{
	// AI space
	pApp->LoadTitle("Loading AI space...");
	AI.Load(Path.Current);

	// Load RPoints
	CInifile::Sect& S = pLevel->ReadSection("respawn_point");
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
		Fvector4	pos;
		int			team;
		const char*	sVal = I->second;
		sscanf(sVal,"%f,%f,%f,%d,%f",&pos.x,&pos.y,&pos.z,&team,&pos.w); pos.y += 0.1f;
		Level().get_team(team).RespawnPoints.push_back(pos);
	}

	tpaPatrolPaths.clear();
	FILE_NAME	fn_game;
	if (Engine.FS.Exist(fn_game, Path.Current, "level.game")) {
		CStream *F = Engine.FS.Open(fn_game);
		CStream *O = F->OpenChunk(WAY_PATH_CHUNK);
		if (O) {
			int chunk = 0;
			for (CStream *OBJ = O->OpenChunk(chunk++); OBJ; OBJ = O->OpenChunk(chunk++)) {
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_VERSION));
				DWORD dw = OBJ->Rword();
				R_ASSERT(dw == WAYOBJECT_VERSION);

				tpaPatrolPaths.resize(tpaPatrolPaths.size() + 1);
				SPatrolPath &tPatrolPath = tpaPatrolPaths[tpaPatrolPaths.size() - 1];

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_NAME));
				OBJ->RstringZ(tPatrolPath.sName);

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_TYPE));
				tPatrolPath.dwType = OBJ->Rdword();

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_POINTS));
				DWORD dwCount = OBJ->Rword();
				tPatrolPath.tpaWayPoints.resize(dwCount);
				for (int i=0; i<dwCount; i++){
					OBJ->Rvector(tPatrolPath.tpaWayPoints[i].tWayPoint);
					tPatrolPath.tpaWayPoints[i].dwFlags = OBJ->Rdword();
					if (tPatrolPath.tpaWayPoints[i].dwFlags & START_WAYPOINT)
						tPatrolPath.dwStartNode = Level().AI.q_LoadSearch(tPatrolPath.tpaWayPoints[i].tWayPoint);
				}

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_LINKS));
				DWORD dwCountL = OBJ->Rword();
				tPatrolPath.tpaWayLinks.resize(dwCountL);
				for ( i=0; i<dwCountL; i++){
					tPatrolPath.tpaWayLinks[i].wFrom = OBJ->Rword();
					tPatrolPath.tpaWayLinks[i].wTo = OBJ->Rword();
				}

				OBJ->Close();

				// sorting links
				bool bOk;
				do {
					bOk = true;
					for ( i=1; i<dwCountL; i++)
						if ((tPatrolPath.tpaWayLinks[i - 1].wFrom > tPatrolPath.tpaWayLinks[i].wFrom) || ((tPatrolPath.tpaWayLinks[i - 1].wFrom == tPatrolPath.tpaWayLinks[i].wFrom) && (tPatrolPath.tpaWayLinks[i - 1].wTo > tPatrolPath.tpaWayLinks[i].wTo))) {
							WORD wTemp = tPatrolPath.tpaWayLinks[i - 1].wFrom;
							tPatrolPath.tpaWayLinks[i - 1].wFrom = tPatrolPath.tpaWayLinks[i].wFrom;
							tPatrolPath.tpaWayLinks[i].wFrom = wTemp;
							wTemp = tPatrolPath.tpaWayLinks[i - 1].wTo;
							tPatrolPath.tpaWayLinks[i - 1].wTo = tPatrolPath.tpaWayLinks[i].wTo;
							tPatrolPath.tpaWayLinks[i].wTo = wTemp;
							bOk = false;
						}
				}
				while (!bOk);
			}
			O->Close();
		}
		Engine.FS.Close(F);
	}

	return TRUE;
}

BOOL CLevel::Load_GameSpecific_After()
{
	return TRUE;
}

