#include "stdafx.h"
#include "HUDmanager.h"
#include "LevelGameDef.h"
#include "ai_space.h"

void CLevel::vfCreateAllPossiblePaths(string64 sName, SPath &tpPatrolPath)
{
	vector<BYTE>		tpaFrom;
	vector<BYTE>		tpaTo;
	vector<Fvector>		tpaPoints;
	vector<Fvector>		tpaDeviations;
	vector<u32>		tpaNodes;

	int i;
//	bool bStop			= false;
	int iStartPoint = -1, iFinishPoint = -1, iCurPoint = 0, iPrevPoint = -1;
	u32 N = tpPatrolPath.tpaWayPoints.size(), dwOneZero = 0, dwZeroOne = 0, dwOneCount = 0, dwTwoCount = 0;

	tpaFrom.resize		(N);
	tpaTo.resize		(N);
	
	tpPatrolPath.dwType = PATH_LOOPED | PATH_BIDIRECTIONAL;
	
	// computing from-to arrays
	for ( i=0; i<(int)N; i++)
		tpaTo[i] = tpaFrom[i] = 0;
	
	for ( i=0; i<(int)tpPatrolPath.tpaWayLinks.size(); i++) {
		tpaTo[tpPatrolPath.tpaWayLinks[i].wTo]++;
		tpaFrom[tpPatrolPath.tpaWayLinks[i].wFrom]++;
	}
	
	// counting types of points
	for ( i=0; i<(int)N; i++) {
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
			if ((!dwOneCount) && (!dwZeroOne) && (!dwOneZero))
				iStartPoint = i;
			else
				if ((dwOneCount == 1) && (!dwZeroOne) && (!dwOneZero))
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
	for ( i=0; i<(int)N; i++) {			
		tpPatrolPath.tpaWayPointIndexes[i] = iCurPoint;
		for (int j=0; j<(int)tpPatrolPath.tpaWayLinks.size(); j++)
			if ((tpPatrolPath.tpaWayLinks[j].wFrom == iCurPoint) && (tpPatrolPath.tpaWayLinks[j].wTo != iPrevPoint)) {
				iPrevPoint = iCurPoint;
				iCurPoint = tpPatrolPath.tpaWayLinks[j].wTo;
				break;
			}
	}

	// creating realistic path
	tpaDeviations.resize(N);
	tpaPoints.resize(N);
	for (int i=0; i<(int)N; i++)
		tpaPoints[i] = tpPatrolPath.tpaWayPoints[tpPatrolPath.tpaWayPointIndexes[i]].tWayPoint;

	getAI().vfCreateFastRealisticPath(tpaPoints,tpPatrolPath.tpaWayPoints[tpPatrolPath.tpaWayPointIndexes[0]].dwNodeID,tpaDeviations,tpPatrolPath.tpaVectors[0],tpaNodes,tpPatrolPath.dwType & PATH_LOOPED);

	// creating variations
	if (!tpPatrolPath.tpaVectors[0].size()) {
		Msg("Patrol path %s was not built - there are not enough nodes to build all the straight lines",sName);
		THROW;
	}
	tpPatrolPath.tpaVectors[1].resize(tpPatrolPath.tpaVectors[0].size());
	tpPatrolPath.tpaVectors[2].resize(tpPatrolPath.tpaVectors[0].size());
			
	float fHalfSubnodeSize = getAI().Header().size*.5f;

	vector<Fvector> &tpaVector0 = tpPatrolPath.tpaVectors[0];
	u32 M = tpaVector0.size();

	for (int I=1; I<3; I++) {
		vector<Fvector> &tpaVector1 = ((I == 1) ? tpPatrolPath.tpaVectors[1] : tpPatrolPath.tpaVectors[2]);
		for (int i=0, j=0, k=0; i<(int)M; i++, j++) {
			
			tpaVector1[j] = tpaVector0[i];

			Fvector tTemp;
			
			if (tpPatrolPath.dwType & PATH_LOOPED) {
				tTemp.sub(tpaVector0[i < (int)M - 1 ? i + 1 : 0], tpaVector0[i]);
				tTemp.y = 0.f;
				if (tTemp.magnitude() < EPS_L) {
					tTemp.sub(tpaVector0[i < (int)M - 2 ? i + 2 : 1], tpaVector0[i]);
					tTemp.y = 0.f;
				}
			}
			else {
				if (i < (int)M - 1)
					tTemp.sub(tpaVector0[i < (int)M - 1 ? i + 1 : 0], tpaVector0[i]);
				else
					tTemp.sub(tpaVector0[i], tpaVector0[i - 1]);
				tTemp.y = 0.f;
			}
			tTemp.normalize();

			if (I == 1)
				tTemp.set(tTemp.z,0,-tTemp.x);
			else
				tTemp.set(-tTemp.z,0,tTemp.x);
			
			tpaVector1[j].add(tTemp);

			for (int m=k; (k < (int)tpaNodes.size()) && (!getAI().bfInsideNode(getAI().Node(tpaNodes[k]),tpaVector0[i])); k++) ;

			if (k >= (int)tpaNodes.size()) {
				k = m;
				tpaVector1.erase(tpaVector1.begin() + j);
				j--;
				continue;
			}

			u32 dwBestNode;
			float fBestCost;
			NodePosition tNodePosition;
			getAI().PackPosition(tNodePosition,tpaVector1[j]);
			getAI().q_Range_Bit_X(tpaNodes[k],tpaVector0[i],4*fHalfSubnodeSize,&tNodePosition,dwBestNode,fBestCost);
			tpaVector1[j].y = getAI().ffGetY(*(getAI().Node(dwBestNode)),tpaVector1[j].x,tpaVector1[j].z);
		}
		if (tpaVector1[0].distance_to(tpaVector1[j - 1]) > EPS_L) {
			tpaVector1.push_back(tpaVector1[0]);
			j++;
		}
		tpaVector1.resize(j);
	}
	if ((tpPatrolPath.dwType & PATH_LOOPED) && (tpaVector0[0].distance_to(tpaVector0[tpaVector0.size() - 1]) > EPS_L))
		tpaVector0.push_back(tpaVector0[0]);
}

BOOL CLevel::Load_GameSpecific_Before()
{
	// AI space
	pApp->LoadTitle	("Loading AI objects...");
	getAI().Load(Path.Current);

	FILE_NAME		fn_game;
	if (Engine.FS.Exist(fn_game, Path.Current, "level.game")) 
	{
		CStream *F = Engine.FS.Open	(fn_game);
		CStream *O = 0;

		// Load WayPoints
		if (0!=(O = F->OpenChunk	(WAY_PATROLPATH_CHUNK)))
		{
			int chunk = 0;
			for (CStream *OBJ = O->OpenChunk(chunk++); OBJ; OBJ = O->OpenChunk(chunk++)) {
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_VERSION));
				u32 dw = OBJ->Rword();
				R_ASSERT(dw == WAYOBJECT_VERSION);

				SPath tPatrolPath;

				string64 sName;
				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_NAME));
				OBJ->RstringZ(sName);

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_POINTS));
				u32 dwCount = OBJ->Rword();
				tPatrolPath.tpaWayPoints.resize(dwCount);
				for (int i=0; i<(int)dwCount; i++){
					OBJ->Rvector(tPatrolPath.tpaWayPoints[i].tWayPoint);
					tPatrolPath.tpaWayPoints[i].dwFlags = OBJ->Rdword();
					tPatrolPath.tpaWayPoints[i].dwNodeID = getAI().q_LoadSearch(tPatrolPath.tpaWayPoints[i].tWayPoint);
				}

				R_ASSERT(OBJ->FindChunk(WAYOBJECT_CHUNK_LINKS));
				u32 dwCountL = OBJ->Rword();
				tPatrolPath.tpaWayLinks.resize(dwCountL);
				for ( i=0; i<(int)dwCountL; i++){
					tPatrolPath.tpaWayLinks[i].wFrom = OBJ->Rword();
					tPatrolPath.tpaWayLinks[i].wTo = OBJ->Rword();
				}

				OBJ->Close();

				// sorting links
				bool bOk;
				do {
					bOk = true;
					for ( i=1; i<(int)dwCountL; i++)
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
			}
			O->Close();
		}
		Engine.FS.Close(F);
	}

	return TRUE;
}

BOOL CLevel::Load_GameSpecific_After()
{
	// only for single...hmmm????
//	m_tpAI_DDD	= xr_new<CAI_DDD> ();
//	m_tpAI_DDD->vfLoad();

	return TRUE;
}

void CLevel::Load_GameSpecific_CFORM	( CDB::TRI* tris, u32 count )
{
	// 1.
	u16		default_id	= (u16)GMLib.GetMaterialIdx("default");

	// 2. Build mapping
	map<u32,u16>		translator;
	translator.insert	(make_pair(u32(-1),default_id));
	u16 idx				= 0;
	for (GameMtlIt I=GMLib.FirstMaterial(); I!=GMLib.LastMaterial(); I++)
	{
		translator.insert(make_pair((*I)->GetID(),idx++));
	}

	// 3.
	for (u32 it=0; it<count; it++)
	{
		CDB::TRI* T						= tris + it;
		map<u32,u16>::iterator index	= translator.find(T->dummy);
		if (index==translator.end())	Debug.fatal	("Game material '%d' not found",T->dummy);
		T->material						= index->second;
	}
}
