////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_temporary.cpp
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation	temporary file for simulating simulations :-)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
using namespace AI;
using namespace ALife;

const char *cpMonsterModels[] = {
	// monsters
	"ent_rat",
	"ent_zombie",
	"ent_dog",
	"ent_controller",
	0
};
const char *cpHumanModels[] = {
	// human beings
	"ent_soldier",
	"ent_stalker",
	"ent_trader",
	0
};
const char *cpWeaponModels[] = {
	// weapon
	"wpn_fn2000",
	"wpn_lr300",
	"wpn_ak74",
	"wpn_hpsa",
	"wpn_pm",
	"wpn_fort",
	"wpn_binoc",
	"wpn_toz34",
	0
};
const char *cpWeaponChModels[] = {
	// wepon charges
	"wpn_fn2000_ch",
	"wpn_lr300_ch",
	"wpn_ak74_ch",
	"wpn_hpsa_ch",
	"wpn_pm_ch",
	"wpn_fort_ch",
	"wpn_toz34_ch",
	0
};
const char *cpEquipmentModels[] = {
	// equipment
	"eq_radio",
	"eq_life_saver",
	"eq_capsule",
	"eq_container",
	"eq_psi_probe",
	"eq_u_detector",
	"eq_lf_detector",
	"eq_medikit",
	"eq_u_medikit",
	"eq_p_suit",
	"eq_st_suit",
	"eq_sc_suit",
	"eq_ar_suit",
	0
};
const char *cpArtefactModels[] = {
	// artefacts
	"art_gravi",
	"art_radio",
	"art_magnet",
	"art_mball",
	"art_black_droplets",
	0
};

//IC bool	bfSpawnPointPredicate(CALifeSpawnPoint *v1, CALifeSpawnPoint *v2)
//{
//	return(v1->m_wGroupID <= v2->m_wGroupID);
//}
//
void CAI_ALife::vfGenerateSpawnPoints(const u32 dwTotalCount, AI::FLOAT_VECTOR &fpFactors)
{
	/**
	vector<bool>				tpMarks;
	tpMarks.resize				(Level().AI.GraphHeader().dwVertexCount);
	tpMarks.assign				(tpMarks.size(),false);
	AI::SGraphVertex			*tpaGraph = Level().AI.m_tpaGraph;
	const char					**cppModels[] = { cpMonsterModels, cpHumanModels, cpWeaponModels, cpWeaponChModels, cpEquipmentModels, cpArtefactModels, 0};
	DWORD_VECTOR				dwpModelCounts;
	DWORD_VECTOR				dwpSpawnCounts;
	
	for (u32 dwModelCount = 0; cppModels[dwModelCount]; dwModelCount++);
	dwpModelCounts.resize		(dwModelCount);
	dwpSpawnCounts.resize		(dwModelCount);
	
	DWORD_IT					B = dwpModelCounts.begin();
	DWORD_IT					E = dwpModelCounts.end();
	DWORD_IT					I = B;
	
	for ( ; I != E; I++)
		for ( *I = 0; cppModels[I - B][*I]; ++*I);

	B							= dwpSpawnCounts.begin();
	E							= dwpSpawnCounts.end();
	I							= B;
	{
		FLOAT_IT				i = fpFactors.begin();
		for ( ; I != E; I++, i++)
			*I = u32((*i)*float(dwTotalCount));
	}
	u32 dwSpawnCount = 0;
	for (I = B; I != E; I++)
		dwSpawnCount += *I;
	
	//dwSpawnCount += 3*dwpSpawnCounts[1];
	m_tpSpawnPoints.resize		(dwSpawnCount);
	m_tSpawnHeader.dwCount		= dwSpawnCount;
	m_tSpawnHeader.dwVersion	= SPAWN_POINT_VERSION;
	u16 wGroupID				= 0;
	::Random.seed				(0);
	int							ii = 0;
	for (I = B; I != E; I++)
		for (int i=0; i<(int)*I; i++, ii++) {
			m_tpSpawnPoints[ii].tNearestGraphPointID		= (u16)::Random.randI(Level().AI.GraphHeader().dwVertexCount);
			bool bOk = false;
			for (int j=0; j<ii; j++)
				if (m_tpSpawnPoints[j].tNearestGraphPointID == m_tpSpawnPoints[ii].tNearestGraphPointID) {
					bOk = true;
					m_tpSpawnPoints[ii].wGroupID = m_tpSpawnPoints[j].wGroupID;
					break;
				}
			if (!bOk)
				m_tpSpawnPoints[ii].wGroupID = wGroupID++;
			j = ::Random.randI(dwpModelCounts[I - B]);
			Memory.mem_copy(m_tpSpawnPoints[ii].caModel,cppModels[I - B][j],(1 + strlen(cppModels[I - B][j]))*sizeof(char));
			m_tpSpawnPoints[ii].ucTeam					= (u8)::Random.randI(255);
			m_tpSpawnPoints[ii].ucSquad					= (u8)::Random.randI(255);
			m_tpSpawnPoints[ii].ucGroup					= (u8)::Random.randI(255);
			if (I == B)
				m_tpSpawnPoints[ii].wCount				= (u16)::Random.randI(5,45);
			else
				m_tpSpawnPoints[ii].wCount				= 1;
			m_tpSpawnPoints[ii].fBirthRadius			= 10.f;
			m_tpSpawnPoints[ii].fBirthProbability		= 1.0f;
			m_tpSpawnPoints[ii].fIncreaseCoefficient	= 1.01f;
			m_tpSpawnPoints[ii].fAnomalyDeathProbability= 0.0f;
			m_tpSpawnPoints[ii].ucRoutePointCount		= 0;
			m_tpSpawnPoints[ii].tpRouteGraphPoints.clear();
			if (I == B) {
				m_tpSpawnPoints[ii].tpRouteGraphPoints.push_back(m_tpSpawnPoints[ii].tNearestGraphPointID);
				tpMarks[m_tpSpawnPoints[ii].tNearestGraphPointID] = true;
				u16				wPoint = m_tpSpawnPoints[ii].tNearestGraphPointID;
				int				wCount = tpaGraph[wPoint].tNeighbourCount;
				AI::SGraphEdge	*tpaEdges = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint].dwEdgeOffset);
				for ( j=0; j<(int)wCount; j++) {
					if (!tpMarks[tpaEdges[j].dwVertexNumber]) {
						m_tpSpawnPoints[ii].tpRouteGraphPoints.push_back((u16)tpaEdges[j].dwVertexNumber);
						tpMarks[tpaEdges[j].dwVertexNumber] = true;
					}
					u32				wPoint1 = tpaEdges[j].dwVertexNumber;
					int				wCount1 = tpaGraph[wPoint1].tNeighbourCount;
					AI::SGraphEdge	*tpaEdges1 = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint1].dwEdgeOffset);
					for (int k=0; k<wCount1; k++)
						if (!tpMarks[tpaEdges1[k].dwVertexNumber]) {
							m_tpSpawnPoints[ii].tpRouteGraphPoints.push_back((u16)tpaEdges1[k].dwVertexNumber);
							tpMarks[tpaEdges1[k].dwVertexNumber] = true;
						}
				}
				m_tpSpawnPoints[ii].ucRoutePointCount			= (u8)m_tpSpawnPoints[ii].tpRouteGraphPoints.size();
				for ( j=0; j<(int)m_tpSpawnPoints[ii].ucRoutePointCount; j++)
					tpMarks[m_tpSpawnPoints[ii].tpRouteGraphPoints[j]] = false;
			}
		}
	/**/
//	m_tSpawnVersion				= SPAWN_POINT_VERSION;
//	u16 wGroupID				= 0;
//	m_tpSpawnPoints.resize		(Level().AI.GraphHeader().dwVertexCount + 2);
//	int							j;
//	SPAWN_P_IT					B = m_tpSpawnPoints.begin();
//	SPAWN_P_IT					E = m_tpSpawnPoints.end() - 2;
//	SPAWN_P_IT					I = B;
//	for ( ; I != E; I++) {
//		(*I)							= xr_new<CALifeSpawnPoint> ();
//		(*I)->m_tNearestGraphPointID	= _GRAPH_ID(I - B);
//		(*I)->m_wGroupID				= wGroupID++;
//		j								= ::Random.randI(5);
//		Memory.mem_copy					((*I)->m_caModel,cpArtefactModels[j],(1 + strlen(cpArtefactModels[j]))*sizeof(char));
//		(*I)->m_ucTeam					= 0;
//		(*I)->m_ucSquad					= 0;
//		(*I)->m_ucGroup					= 0;
//		(*I)->m_wCount					= 1;
//		(*I)->m_fBirthRadius			= 10.f;
//		(*I)->m_fBirthProbability		= 1.0f;
//		(*I)->m_fIncreaseCoefficient	= 1.0f;
//		(*I)->m_tpRouteGraphPoints.clear();
//	}
//	(*I)							= xr_new<CALifeSpawnPoint> ();
//	(*I)->m_tNearestGraphPointID	= _GRAPH_ID(::Random.randI(Level().AI.GraphHeader().dwVertexCount));
//	(*I)->m_wGroupID				= wGroupID++;
//	j								= 1;
//	Memory.mem_copy					((*I)->m_caModel,cpHumanModels[j],(1 + strlen(cpHumanModels[j]))*sizeof(char));
//	(*I)->m_ucTeam					= 1;
//	(*I)->m_ucSquad					= 0;
//	(*I)->m_ucGroup					= 0;
//	(*I)->m_wCount					= 1;
//	(*I)->m_fBirthRadius			= 10.f;
//	(*I)->m_fBirthProbability		= 1.0f;
//	(*I)->m_fIncreaseCoefficient	= 1.0f;
//	(*I)->m_tpRouteGraphPoints.clear();
//
//	I++;
//
//	(*I)							= xr_new<CALifeSpawnPoint> ();
//	(*I)->m_tNearestGraphPointID	= _GRAPH_ID(::Random.randI(Level().AI.GraphHeader().dwVertexCount));
//	(*I)->m_wGroupID				= wGroupID++;
//	j								= 2;
//	Memory.mem_copy					((*I)->m_caModel,cpHumanModels[j],(1 + strlen(cpHumanModels[j]))*sizeof(char));
//	(*I)->m_ucTeam					= 1;
//	(*I)->m_ucSquad					= 0;
//	(*I)->m_ucGroup					= 0;
//	(*I)->m_wCount					= 1;
//	(*I)->m_fBirthRadius			= 10.f;
//	(*I)->m_fBirthProbability		= 1.0f;
//	(*I)->m_fIncreaseCoefficient	= 1.0f;
//	(*I)->m_tpRouteGraphPoints.clear();
//	/**/
//	sort(m_tpSpawnPoints.begin(),m_tpSpawnPoints.end(),bfSpawnPointPredicate);
}

void CAI_ALife::vfSaveSpawnPoints()
{
//	CFS_Memory	tStream;
//	tStream.open_chunk	(SPAWN_POINT_CHUNK_VERSION);
//	tStream.write		(&m_tSpawnVersion,sizeof(m_tSpawnVersion));
//	tStream.close_chunk	();
//	tStream.open_chunk	(SPAWN_POINT_CHUNK_DATA);
//	tStream.Wdword		(m_tpSpawnPoints.size());
//	SPAWN_P_IT			I = m_tpSpawnPoints.begin();
//	SPAWN_P_IT			E = m_tpSpawnPoints.end();
//	for ( ; I != E; I++) {
//		tStream.Wword	((*I)->m_tNearestGraphPointID);
//		tStream.Wstring	((*I)->m_caModel);
//		tStream.Wbyte	((*I)->m_ucTeam);
//		tStream.Wbyte	((*I)->m_ucSquad);
//		tStream.Wbyte	((*I)->m_ucGroup);
//		tStream.Wword	((*I)->m_wGroupID);
//		tStream.Wword	((*I)->m_wCount);
//		tStream.Wfloat	((*I)->m_fBirthRadius);
//		tStream.Wfloat	((*I)->m_fBirthProbability);
//		tStream.Wfloat	((*I)->m_fIncreaseCoefficient);
//		save_base_vector((*I)->m_tpRouteGraphPoints,tStream);
//	}
//	tStream.close_chunk	();
//	tStream.SaveTo		("game.spawn",0);
}

#ifdef DEBUG
void CAI_ALife::vfRandomizeGraphTerrain()
{
	SGraphVertex *tpaGraph = (SGraphVertex *)xr_malloc(Level().AI.m_tpGraphVFS->Length());
	Memory.mem_copy(tpaGraph,(BYTE *)Level().AI.m_tpaGraph - sizeof(SGraphHeader),Level().AI.m_tpGraphVFS->Length());
	tpaGraph = (SGraphVertex *)((BYTE *)tpaGraph + sizeof(SGraphHeader));
	for (int i=0, m=0; i<(int)Level().AI.GraphHeader().dwVertexCount; i++)
		if (!tpaGraph[i].tVertexType) {
			m = (m + 1) & 0xff;
			tpaGraph[i].tVertexType = (u16)m;
			int			wCount = tpaGraph[i].tNeighbourCount;
			SGraphEdge	*tpaEdges = (SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[i].dwEdgeOffset);
			for (int j=0; j<(int)wCount; j++)
				if (!tpaGraph[tpaEdges[j].dwVertexNumber].tVertexType) {
					tpaGraph[tpaEdges[j].dwVertexNumber].tVertexType = (u16)m;
					int			wCount1 = tpaGraph[tpaEdges[j].dwVertexNumber].tNeighbourCount;
					SGraphEdge	*tpaEdges1 = (SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[tpaEdges[j].dwVertexNumber].dwEdgeOffset);
					for (int k=0; k<wCount1; k++)
						if (!tpaGraph[tpaEdges1[k].dwVertexNumber].tVertexType)
							tpaGraph[tpaEdges1[k].dwVertexNumber].tVertexType = (u16)m;
				}
		}
	tpaGraph = (SGraphVertex *)((BYTE *)tpaGraph - sizeof(SGraphHeader));
	FILE *F = fopen("level.graph","wb");
	fwrite(tpaGraph,1,Level().AI.m_tpGraphVFS->Length(),F);
	fclose(F);
	xr_free(tpaGraph);
}
#endif

void CAI_ALife::Generate()
{
//	SPAWN_P_IT	B = m_tpSpawnPoints.begin();
//	SPAWN_P_IT	E  = m_tpSpawnPoints.end();
//	for (SPAWN_P_IT I = B ; I != E; ) {
//		u16	wGroupID = I->wGroupID;
//		float fSum = I->fBirthProbability;
//		for (SPAWN_P_IT j= I + 1; (j != E) && (j->wGroupID == wGroupID); j++)
//			fSum += j->fBirthProbability;
//		float fProbability = ::Random.randF(0,fSum);
//		fSum = I->fBirthProbability;
//		SPAWN_P_IT m = j, k = I;
//		for ( j= I + 1; (j != E) && (j->wGroupID == wGroupID); j++) {
//			fSum += j->fBirthProbability;
//			if (fSum > fProbability) {
//				k = j;
//				break;
//			}
//		}
//		vfCreateNewDynamicObject(k);
//		I = m;	
//	}

//	vfCreateNewDynamicObject	(m_tpSpawnPoints.end() - 2);
//	vfCreateNewDynamicObject	(m_tpSpawnPoints.end() - 1);

	m_tALifeVersion				= ALIFE_VERSION;
	m_tGameTime					= u64(Level().timeServer());
}