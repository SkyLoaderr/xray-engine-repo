#include "stdafx.h"
#include "HUDmanager.h"
#include "WayPointDef.h"

void CLevel::vfCreateAllPossiblePaths(string64 sName, SPath &tpPatrolPath)
{
	vector<BYTE>		tpaFrom;
	vector<BYTE>		tpaTo;
	vector<Fvector>		tpaPoints;
	vector<Fvector>		tpaDeviations;
	vector<DWORD>		tpaNodes;

	int i;
	bool bStop = false;
	int iStartPoint = -1, iFinishPoint = -1, iCurPoint = 0, iPrevPoint = -1;
	DWORD N = tpPatrolPath.tpaWayPoints.size(), dwOneZero = 0, dwZeroOne = 0, dwOneCount = 0, dwTwoCount = 0;

	tpaFrom.resize(N);
	tpaTo.resize(N);
	
	tpPatrolPath.dwType = PATH_LOOPED | PATH_BIDIRECTIONAL;
	
	// computing from-to arrays
	for ( i=0; i<N; i++)
		tpaTo[i] = tpaFrom[i] = 0;
	
	for ( i=0; i<N; i++) {
		tpaTo[tpPatrolPath.tpaWayLinks[i].wTo]++;
		tpaFrom[tpPatrolPath.tpaWayLinks[i].wFrom]++;
	}
	
	// counting types of points
	for ( i=0; i<N; i++) {
		if (tpaTo[i] > 2) {
			Msg("Patrol path %s : invalid count of incoming links (%d) for point %d [%.2f,%.2f,%.2f]",sName,tpaTo[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
			THROW;
		}
		if (tpaFrom[i] > 2) {
			Msg("Patrol path %s : invalid count of outcoming links (%d) for point %d [%.2f,%.2f,%.2f]",sName,tpaFrom[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
			THROW;
		}
		if ((tpaTo[i] == 1) && (tpaFrom[i] == 0)) {
			if (dwOneZero) {
				Msg("Patrol path %s : invalid count of start points [%.2f,%.2f,%.2f]",sName,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
				THROW;
			}
			dwOneZero++;
			iFinishPoint = i;
		}
		
		if ((tpaTo[i] == 0) && (tpaFrom[i] == 1)) {
			if (dwZeroOne) {
				Msg("Patrol path %s : invalid count of finish points [%.2f,%.2f,%.2f]",sName,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
				THROW;
			}
			dwZeroOne++;
			iStartPoint = i;
		}
		
		if ((tpaTo[i] == 1) && (tpaFrom[i] == 1)) {
			if (!dwOneCount)
				iStartPoint = i;
			else
				if (dwOneCount == 1)
					iFinishPoint = i;
			dwOneCount++;
		}
		
		if ((tpaTo[i] == 2) && (tpaFrom[i] == 2))
			dwTwoCount++;
	}
	
	// checking for supported path types
	if (!(dwOneZero + dwZeroOne)) {
		if ((dwOneCount == 2) && (dwTwoCount == N - 2)) {
			iCurPoint = iStartPoint;
			tpPatrolPath.dwType ^= PATH_LOOPED;
		}
		else
			if (dwOneCount == N)
				tpPatrolPath.dwType ^= PATH_BIDIRECTIONAL;
			else
				if (dwTwoCount != N) {
					Msg("Patrol path %s : invalid count of outcoming links (%d) for point %d [%.2f,%.2f,%.2f]",sName,tpaFrom[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
					THROW;
				}
	}
	else
		if ((dwOneCount != N - 2) || (dwOneZero != 1) || (dwZeroOne != 1)) {
			Msg("Patrol path %s : invalid count of outcoming links (%d) for point %d [%.2f,%.2f,%.2f] in non-looped one-directional path",sName,tpaFrom[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
			THROW;
		}
		else {
			iCurPoint = iStartPoint;
			tpPatrolPath.dwType ^= PATH_LOOPED ^ PATH_BIDIRECTIONAL;
		}

	// building point sequencies and path
	tpPatrolPath.tpaWayPointIndexes.resize(N);
	for ( i=0; i<N; i++) {			
		tpPatrolPath.tpaWayPointIndexes[i] = iCurPoint;
		for (int j=0; j<tpPatrolPath.tpaWayLinks.size(); j++)
			if ((tpPatrolPath.tpaWayLinks[j].wFrom == iCurPoint) && (tpPatrolPath.tpaWayLinks[j].wTo != iPrevPoint)) {
				iPrevPoint = iCurPoint;
				iCurPoint = tpPatrolPath.tpaWayLinks[j].wTo;
				break;
			}
	}

	// creating realistic path
	tpaDeviations.resize(N);
	tpaPoints.resize(N);
	for (int i=0; i<N; i++)
		tpaPoints[i] = tpPatrolPath.tpaWayPoints[tpPatrolPath.tpaWayPointIndexes[i]].tWayPoint;

	AI.vfCreateFastRealisticPath(tpaPoints,tpPatrolPath.tpaWayPoints[tpPatrolPath.tpaWayPointIndexes[0]].dwNodeID,tpaDeviations,tpPatrolPath.tpaVectors[0],tpaNodes,tpPatrolPath.dwType & PATH_LOOPED);

	// creating variations
	tpPatrolPath.tpaVectors[1].resize(tpPatrolPath.tpaVectors[0].size());
	tpPatrolPath.tpaVectors[2].resize(tpPatrolPath.tpaVectors[0].size());
			
	float fHalfSubnodeSize = AI.GetHeader().size*.5f;

	vector<Fvector> &tpaVector0 = tpPatrolPath.tpaVectors[0];
	DWORD M = tpaVector0.size();

	for (int I=1; I<3; I++) {
		vector<Fvector> &tpaVector1 = I == 1 ? tpPatrolPath.tpaVectors[1] : tpPatrolPath.tpaVectors[2];
		for (int i=0, j=0, k=0; i<M; i++, j++) {
			
			tpaVector1[j] = tpaVector0[i];

			Fvector tTemp;
			
			if (tpPatrolPath.dwType & PATH_LOOPED)
				tTemp.sub(tpaVector0[i < M - 1 ? i + 1 : 0], tpaVector0[i]);
			else
				if (i < M - 1)
					tTemp.sub(tpaVector0[i < M - 1 ? i + 1 : 0], tpaVector0[i]);
				else
					tTemp.sub(tpaVector0[i], tpaVector0[i - 1]);

			if (tTemp.magnitude() < .1f) {
				j--;
				continue;
			}

			tTemp.y = 0.f;
			tTemp.normalize();

			tTemp.set(tTemp.z,0,-tTemp.x);
			
			tpaVector1[j].add(tTemp);

			if ((j > 1) && (tpaVector1[j].distance_to(tpaVector1[j - 2]) <= fHalfSubnodeSize + tpaVector1[j - 1].distance_to(tpaVector1[j - 2]))) {
				Fvector tPrevious = tpaVector1[j - 2];
				Fvector tCurrent = tpaVector1[j - 1];
				Fvector tNext = tpaVector1[j];
				Fvector tTemp1, tTemp2;
				tTemp1.sub(tCurrent,tPrevious);
				tTemp2.sub(tNext,tCurrent);
				tTemp1.normalize_safe();
				tTemp1.y = tTemp2.y = 0;
				tTemp2.normalize_safe();
				float fAlpha = tTemp1.dotproduct(tTemp2);
				clamp(fAlpha, -.99999f, +.99999f);
				if ((acosf(fAlpha) < PI_DIV_8*.375f) || (acosf(fAlpha) > 2*PI_DIV_8*.375f)) {
					j--;
					continue;
				}
			}
			
			for (int m=k; (k < tpaNodes.size()) && (!AI.bfInsideNode(AI.Node(tpaNodes[k]),tpaVector0[i],fHalfSubnodeSize)); k++) ;

			if (k >= tpaNodes.size()) {
				k = m;
				tpaVector1.erase(tpaVector1.begin() + j);
				j--;
				continue;
			}

			DWORD dwBestNode;
			float fBestCost;
			NodePosition tNodePosition;
			AI.PackPosition(tNodePosition,tpaVector1[j]);
			AI.q_Range_Bit_X(tpaNodes[k],tpaVector0[i],4*fHalfSubnodeSize,&tNodePosition,dwBestNode,fBestCost);
			tpaVector1[j].y = AI.ffGetY(*(AI.Node(dwBestNode)),tpaVector1[j].x,tpaVector1[j].z);
		}
	}
}

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

				SPath tPatrolPath;

				string64 sName;
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_NAME));
				OBJ->RstringZ(sName);

				DWORD dwType;
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_TYPE));
				dwType = OBJ->Rdword();

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_POINTS));
				DWORD dwCount = OBJ->Rword();
				tPatrolPath.tpaWayPoints.resize(dwCount);
				for (int i=0; i<dwCount; i++){
					OBJ->Rvector(tPatrolPath.tpaWayPoints[i].tWayPoint);
					tPatrolPath.tpaWayPoints[i].dwFlags = OBJ->Rdword();
					tPatrolPath.tpaWayPoints[i].dwNodeID = AI.q_LoadSearch(tPatrolPath.tpaWayPoints[i].tWayPoint);
				}

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_LINKS));
				DWORD dwCountL = OBJ->Rword();
				tPatrolPath.tpaWayLinks.resize(dwCountL);
				for ( i=0; i<dwCountL; i++){
					tPatrolPath.tpaWayLinks[i].wFrom = OBJ->Rword();
					tPatrolPath.tpaWayLinks[i].wTo = OBJ->Rword();
				}

				OBJ->Close();

				switch (dwType) {
					case wtPatrolPath : {
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

						m_PatrolPaths[sName] = tPatrolPath;
						
						vfCreateAllPossiblePaths(sName, m_PatrolPaths[sName]);
						break;
					}
					default :
						THROW;
				}
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

