////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#define ALIFE_SUPPORT_CONSOLE_COMMANDS

#include "ai_alife_interfaces.h"

class CSE_ALifeGameTime : public IPureALifeLSObject {
public:
	_TIME_ID						m_tGameTime;
	_TIME_ID						m_tLastSurgeTime;
	_TIME_ID						m_tNextSurgeTime;
	float							m_fTimeFactor;
	u32								m_dwStartTime;
	u64								m_qwStartProcessorCycle;

	virtual void					Save(IWriter	&tMemoryStream)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Device.TimerAsync();
		tMemoryStream.open_chunk	(GAME_TIME_CHUNK_DATA);
		tMemoryStream.w				(&m_tGameTime,		sizeof(m_tGameTime));
		tMemoryStream.w				(&m_tLastSurgeTime,	sizeof(m_tLastSurgeTime));
		tMemoryStream.w				(&m_tNextSurgeTime,	sizeof(m_tNextSurgeTime));
		tMemoryStream.w_float		(m_fTimeFactor);
		tMemoryStream.close_chunk	();
	};
	
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT2					(tFileStream.find_chunk(GAME_TIME_CHUNK_DATA),"Can't find chunk GAME_TIME_CHUNK_DATA!");
		tFileStream.r				(&m_tGameTime,		sizeof(m_tGameTime));
		tFileStream.r				(&m_tLastSurgeTime,	sizeof(m_tLastSurgeTime));
		tFileStream.r				(&m_tNextSurgeTime,	sizeof(m_tNextSurgeTime));
		m_fTimeFactor				= tFileStream.r_float();
		m_qwStartProcessorCycle		= CPU::GetCycleCount();
		m_dwStartTime				= Device.TimerAsync();
	};
	
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
	IC void							vfSetTimeFactor(float fTimeFactor)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Device.TimerAsync();
		m_fTimeFactor				= fTimeFactor;
	};
#endif

	IC _TIME_ID						tfGetGameTime()
	{
		return						(m_tGameTime + iFloor(m_fTimeFactor*float(Device.TimerAsync() - m_dwStartTime)));
	};
};

class CSE_ALifeHeader : public IPureALifeLSObject {
public:
	u32								m_tALifeVersion;
	EZoneState						m_tZoneState;
	
									CSE_ALifeHeader()
	{
		m_tALifeVersion				= ALIFE_VERSION;
		m_tZoneState				= eZoneStateAfterSurge;
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
		tMemoryStream.open_chunk	(ALIFE_CHUNK_DATA);
		tMemoryStream.w				(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		tMemoryStream.w				(&m_tZoneState,		sizeof(m_tZoneState));
		tMemoryStream.close_chunk	();
	}
	
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT2					(tFileStream.find_chunk(ALIFE_CHUNK_DATA),"Can't find chunk ALIFE_CHUNK_DATA");
		tFileStream.r				(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		R_ASSERT2					(m_tALifeVersion == ALIFE_VERSION,"ALife version mismatch!");
		tFileStream.r				(&m_tZoneState,		sizeof(m_tZoneState));
	};
};

class CSE_ALifeSpawnHeader : public IPureALifeLObject {
public:
	u32								m_tSpawnVersion;
	u32								m_dwSpawnCount;
	u32								m_dwLevelCount;
	
	virtual void					Load(IReader	&tFileStream);
};

class CSE_ALifeKnownAnomaly : public IPureALifeLSObject {
public:
	EAnomalousZoneType				m_tAnomalousZoneType;
	float							m_fAnomalyPower;
	float							m_fDistance;

									CSE_ALifeKnownAnomaly()
	{
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
		tMemoryStream.w				(&m_tAnomalousZoneType,	sizeof(m_tAnomalousZoneType));
		tMemoryStream.w_float		(m_fAnomalyPower);
		tMemoryStream.w_float		(m_fDistance);
	}
	
	virtual void					Load(IReader	&tFileStream)
	{
		tFileStream.r				(&m_tAnomalousZoneType,	sizeof(m_tAnomalousZoneType));
		m_fAnomalyPower				= tFileStream.r_float();
		m_fDistance					= tFileStream.r_float();
	};
};

class CSE_ALifeArtefactDemand : public IPureALifeLSObject {
public:
	LPCSTR							m_caSection;
	u32								m_dwMinArtefactCount;
	u32								m_dwMaxArtefactCount;
	u32								m_dwMinArtefactPrice;
	u32								m_dwMaxArtefactPrice;
	u32								m_dwRealArtefactCount;
	u32								m_dwRealArtefactPrice;

									CSE_ALifeArtefactDemand(LPCSTR caSection, u32 dwMinArtefactCount, u32 dwMaxArtefactCount, u32 dwMinArtefactPrice, u32 dwMaxArtefactPrice) :	m_caSection(caSection), m_dwMinArtefactCount(dwMinArtefactCount), m_dwMaxArtefactCount(dwMaxArtefactCount), m_dwMinArtefactPrice(dwMinArtefactPrice), m_dwMaxArtefactPrice(dwMaxArtefactPrice),	m_dwRealArtefactCount(0), m_dwRealArtefactPrice(0)
	{
	}
	
	virtual							~CSE_ALifeArtefactDemand()
	{
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
		tMemoryStream.w_u32			(m_dwRealArtefactCount);
		tMemoryStream.w_u32			(m_dwRealArtefactPrice);
	}

	virtual void					Load(IReader	&tFileStream)
	{
		m_dwRealArtefactCount		= tFileStream.r_u32();
		m_dwRealArtefactPrice		= tFileStream.r_u32();
	};
};

class CSE_ALifeArtefactNeed {
public:
	LPCSTR							m_caSection;
	u32								m_dwArtefactCount;

									CSE_ALifeArtefactNeed(LPCSTR caSection, u32 dwArtefactCount) :	m_caSection(caSection), m_dwArtefactCount(dwArtefactCount)
	{
	}

	virtual							~CSE_ALifeArtefactNeed()
	{
	}
};

class CSE_ALifeDiscovery : public IPureALifeLSObject {
public:
	LPCSTR							m_caDiscoveryIdentifier;
	float							m_fSuccessProbability;
	float							m_fDestroyProbability;
	_TIME_ID						m_tFreezeTime;
	_TIME_ID						m_tResearchTime;
	DEMAND_P_VECTOR					m_tpArtefactDemand;
	NEED_P_VECTOR					m_tpArtefactNeed;


									CSE_ALifeDiscovery(LPCSTR caSection)
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
	}

	virtual							~CSE_ALifeDiscovery()
	{
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
		{
			DEMAND_P_IT				I = m_tpArtefactDemand.begin();
			DEMAND_P_IT				E = m_tpArtefactDemand.end();
			for ( ; I != E; I++)
				(*I)->Save			(tMemoryStream);
		}
	}

	virtual void					Load(IReader	&tFileStream)
	{
		{
			DEMAND_P_IT				I = m_tpArtefactDemand.begin();
			DEMAND_P_IT				E = m_tpArtefactDemand.end();
			for ( ; I != E; I++)
				(*I)->Load			(tFileStream);
		}
	};
};

class CSE_ALifeOrganization : public IPureALifeLSObject {
public:
	LPCSTR							m_caOrganizationIdentifier;
	//ARTEFACT_VECTOR					m_tpArtefacts;


									CSE_ALifeOrganization()
	{
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
	}

	virtual void					Load(IReader	&tFileStream)
	{
	};
};
