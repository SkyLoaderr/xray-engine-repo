#include "stdafx.h"
#include "HUDmanager.h"
#include "LevelGameDef.h"
#include "ai_space.h"
#include "ai\ai_selector_template.h"
#include "..\PGObject.h"

void CLevel::vfCreateAllPossiblePaths(string64 sName, SPath &tpPatrolPath)
{
	vector<BYTE>		tpaFrom;
	vector<BYTE>		tpaTo;
	vector<Fvector>		tpaPoints;
	vector<Fvector>		tpaDeviations;
	vector<u32>			tpaNodes;

	int i;
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
		if (tpaTo[i] > 2)
			Debug.fatal("Patrol path %s : invalid count of incoming links (%d) for point %d [%.2f,%.2f,%.2f]",sName,tpaTo[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
		if (tpaFrom[i] > 2)
			Debug.fatal("Patrol path %s : invalid count of outcoming links (%d) for point %d [%.2f,%.2f,%.2f]",sName,tpaFrom[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
		if ((tpaTo[i] == 1) && (tpaFrom[i] == 0)) {
			if (dwOneZero)
				Debug.fatal("Patrol path %s : invalid count of start points [%.2f,%.2f,%.2f]",sName,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
			dwOneZero++;
			iFinishPoint = i;
		}
		
		if ((tpaTo[i] == 0) && (tpaFrom[i] == 1)) {
			if (dwZeroOne)
				Debug.fatal("Patrol path %s : invalid count of finish points [%.2f,%.2f,%.2f]",sName,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
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
				if (dwTwoCount != N)
					Debug.fatal("Patrol path %s : invalid count of outcoming links (%d) for point %d [%.2f,%.2f,%.2f]",sName,tpaFrom[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
	}
	else
		if ((dwOneCount != N - 2) || (dwOneZero != 1) || (dwZeroOne != 1))
			Debug.fatal("Patrol path %s : invalid count of outcoming links (%d) for point %d [%.2f,%.2f,%.2f] in non-looped one-directional path",sName,tpaFrom[i],i,tpPatrolPath.tpaWayPoints[i].tWayPoint.x,tpPatrolPath.tpaWayPoints[i].tWayPoint.y,tpPatrolPath.tpaWayPoints[i].tWayPoint.z);
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
	if (!tpPatrolPath.tpaVectors[0].size())
		Debug.fatal("Patrol path %s was not built - there are not enough nodes to build all the straight lines",sName);
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

			CAI_NodeEvaluatorTemplate<aiSearchRange | aiInsideNode> tSearch;
			tSearch.m_fSearchRange = 4*fHalfSubnodeSize;
			tSearch.m_dwStartNode = tpaNodes[k];
			tSearch.m_tStartPosition = tpaVector0[i];
			tSearch.vfShallowGraphSearch(getAI().q_mark_bit);
//			getAI().q_Range_Bit_X(tpaNodes[k],tpaVector0[i],4*fHalfSubnodeSize,&tNodePosition,dwBestNode,fBestCost);
			tpaVector1[j].y = getAI().ffGetY(*(getAI().Node(tSearch.m_dwBestNode)),tpaVector1[j].x,tpaVector1[j].z);
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
	getAI().Load	();

	string256		fn_game;
	if (FS.exist(fn_game, "$level$", "level.game")) {
		IReader *F = FS.r_open	(fn_game);
		IReader *O = 0;

		// Load WayPoints
		if (0!=(O = F->open_chunk	(WAY_PATROLPATH_CHUNK))) { 
			int chunk = 0;
			for (IReader *OBJ = O->open_chunk(chunk++); OBJ; OBJ = O->open_chunk(chunk++)) {
				R_ASSERT(OBJ->find_chunk(WAYOBJECT_CHUNK_VERSION));
				u32 dw = OBJ->r_u16();
				R_ASSERT(dw == WAYOBJECT_VERSION);

				SPath tPatrolPath;

				string64 sName;
				R_ASSERT(OBJ->find_chunk(WAYOBJECT_CHUNK_NAME));
				OBJ->r_stringZ(sName);

				R_ASSERT(OBJ->find_chunk(WAYOBJECT_CHUNK_POINTS));
				u32 dwCount = OBJ->r_u16();
				tPatrolPath.tpaWayPoints.resize(dwCount);
				for (int i=0; i<(int)dwCount; i++){
					OBJ->r_fvector3(tPatrolPath.tpaWayPoints[i].tWayPoint);
					tPatrolPath.tpaWayPoints[i].dwFlags = OBJ->r_u32();
					tPatrolPath.tpaWayPoints[i].dwNodeID = getAI().q_LoadSearch(tPatrolPath.tpaWayPoints[i].tWayPoint);
				}

				R_ASSERT(OBJ->find_chunk(WAYOBJECT_CHUNK_LINKS));
				u32 dwCountL = OBJ->r_u16();
				tPatrolPath.tpaWayLinks.resize(dwCountL);
				for ( i=0; i<(int)dwCountL; i++){
					tPatrolPath.tpaWayLinks[i].wFrom = OBJ->r_u16();
					tPatrolPath.tpaWayLinks[i].wTo = OBJ->r_u16();
				}

				OBJ->close();

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
			O->close();
		}
		FS.r_close(F);
	}

	return TRUE;
}

BOOL CLevel::Load_GameSpecific_After()
{
	// loading static particles
	string256		fn_game;
	if (FS.exist(fn_game, "$level$", "level.ps_static")) {
		IReader *F = FS.r_open	(fn_game);
		IRender_Sector* S;
		CPGObject*		pStaticPG;
		int				chunk = 0;
		string256		ref_name;
		Fmatrix			transform;
		for (IReader *OBJ = F->open_chunk(chunk++); OBJ; OBJ = F->open_chunk(chunk++)){
			OBJ->r_stringZ				(ref_name);
			OBJ->r						(&transform,sizeof(Fmatrix));transform.c.y+=0.01f;
			S							= ::Render->detectSector	(transform.c);
			pStaticPG					= xr_new<CPGObject>			(ref_name,S,false);
			pStaticPG->UpdateParent		(transform);
			pStaticPG->Play				();
			m_StaticParticles.push_back	(pStaticPG);
		}
	}
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
