////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.cpp
//	Created 	: 23.06.2003
//  Modified 	: 23.06.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_registries.h"
#include "ai_alife_templates.h"

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "..\\xrLevel.h"
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeSpawnHeader
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeSpawnHeader::Load	(IReader	&tFileStream)
{
	R_ASSERT2					(tFileStream.find_chunk(SPAWN_POINT_CHUNK_VERSION),"Can't find chunk SPAWN_POINT_CHUNK_VERSION!");
	m_tSpawnVersion				= tFileStream.r_u32();
	R_ASSERT2					(m_tSpawnVersion == XRAI_CURRENT_VERSION,"'game.spawn' version mismatch!");
	m_dwSpawnCount				= tFileStream.r_u32();
	m_dwLevelCount				= tFileStream.r_u32();
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeDiscovery
////////////////////////////////////////////////////////////////////////////
CSE_ALifeDiscovery::CSE_ALifeDiscovery(LPCSTR caSection)
{
	m_caDiscoveryIdentifier		= pSettings->r_string	(caSection,"name");
	m_fSuccessProbability		= pSettings->r_float	(caSection,"success_probability");
	m_fDestroyProbability		= pSettings->r_float	(caSection,"destroy_probability");
	m_tFreezeTime				= (_TIME_ID)pSettings->r_u32(caSection,"destroy_freeze_time")*24*60*1000;
	m_tResearchTime				= (_TIME_ID)pSettings->r_u32(caSection,"research_time")*24*60*1000;

	LPCSTR						S;
	string64					S1;
	{
		S						= pSettings->r_string	(caSection,"demand_on_success");
		R_ASSERT2				(!(_GetItemCount(S) % 5),"Invalid argument count in the discovery object section!");
		m_tpArtefactDemand.resize(_GetItemCount(S)/5);
		DEMAND_P_IT				B = m_tpArtefactDemand.begin(), I = B;
		DEMAND_P_IT				E = m_tpArtefactDemand.end();
		for ( ; I != E; I++)
			*I = xr_new<CSE_ALifeArtefactDemand>(_GetItem(S,5*int(I - B) + 0,S1),atoi(_GetItem(S,5*int(I - B) + 1,S1)),atoi(_GetItem(S,5*int(I - B) + 2,S1)),atoi(_GetItem(S,5*int(I - B) + 3,S1)),atoi(_GetItem(S,5*int(I - B) + 4,S1)));
	}
	{
		S						= pSettings->r_string	(caSection,"artefacts");
		R_ASSERT2				(!(_GetItemCount(S) % 2),"Invalid argument count in the discovery object section!");
		m_tpArtefactNeed.resize	(_GetItemCount(S)/2);
		NEED_P_IT				B = m_tpArtefactNeed.begin(), I = B;
		NEED_P_IT				E = m_tpArtefactNeed.end();
		for ( ; I != E; I++)
			*I = xr_new<CSE_ALifeArtefactNeed>(_GetItem(S,2*int(I - B) + 0,S1),atoi(_GetItem(S,2*int(I - B) + 1,S1)));
	}
	{
		S						= pSettings->r_string	(caSection,"discovery_dependence");
		m_tpDependency.resize	(_GetItemCount(S));
		LPCSTR_IT				B = m_tpDependency.begin(), I = B;
		LPCSTR_IT				E = m_tpDependency.end();
		for ( ; I != E; I++) {
			_GetItem			(S,int(I - B),S1);
			*I					= (char*)xr_malloc((strlen(S1) + 1)*sizeof(char));
			strcpy				((char*)(*I),S1);
		}
	}
	{
		S						= pSettings->r_string	(caSection,"class_ids");
		m_tpClassIDs.resize		(_GetItemCount(S));
		CLSID_IT				B = m_tpClassIDs.begin(), I = B;
		CLSID_IT				E = m_tpClassIDs.end();
		for ( ; I != E; I++) {
			_GetItem(S,int(I - B),S1);
			R_ASSERT2			(strlen(S1) == 8,"Invalid class identifier!");
			*I					= MK_CLSID(S1[0],S1[1],S1[2],S1[3],S1[4],S1[5],S1[6],S1[7]);
		}
	}
}

CSE_ALifeDiscovery::~CSE_ALifeDiscovery()
{
	free_object_vector			(m_tpArtefactDemand);
	free_object_vector			(m_tpArtefactNeed);
	free_malloc_vector			(m_tpDependency);
}

void CSE_ALifeDiscovery::Save	(IWriter &tMemoryStream)
{
	{
		DEMAND_P_IT				I = m_tpArtefactDemand.begin();
		DEMAND_P_IT				E = m_tpArtefactDemand.end();
		for ( ; I != E; I++)
			(*I)->Save			(tMemoryStream);
	}
}

void CSE_ALifeDiscovery::Load	(IReader &tFileStream)
{
	{
		DEMAND_P_IT				I = m_tpArtefactDemand.begin();
		DEMAND_P_IT				E = m_tpArtefactDemand.end();
		for ( ; I != E; I++)
			(*I)->Load			(tFileStream);
	}
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeOrganization
////////////////////////////////////////////////////////////////////////////
CSE_ALifeOrganization::CSE_ALifeOrganization(LPCSTR caSection)
{
	LPCSTR						S;
	string64					S1;
	S							= pSettings->r_string	(caSection,"discoveries");
	m_tpPossibleDiscoveries.resize(_GetItemCount(S));
	LPCSTR_IT					B = m_tpPossibleDiscoveries.begin(), I = B;
	LPCSTR_IT					E = m_tpPossibleDiscoveries.end();
	for ( ; I != E; I++) {
		_GetItem				(S,int(I - B),S1);
		*I						= (char*)xr_malloc((strlen(S1) + 1)*sizeof(char));
		strcpy					((char*)(*I),S1);
	}
	m_fJoinProbability			= pSettings->r_float	(caSection,"join_probability");
	m_fLeftProbability			= pSettings->r_float	(caSection,"left_probability");
}

CSE_ALifeOrganization::~CSE_ALifeOrganization()
{
	free_malloc_vector			(m_tpPossibleDiscoveries);
}

void CSE_ALifeOrganization::Save(IWriter &tMemoryStream)
{
}

void CSE_ALifeOrganization::Load(IReader &tFileStream)
{
}
