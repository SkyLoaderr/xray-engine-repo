////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_organization.cpp
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife organization class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_organization.h"
#include "object_broker.h"

using namespace ALife;

CALifeOrganization::CALifeOrganization	()
{
	R_ASSERT2					(false,"Default constructor cannot be called explicitly!");
}

CALifeOrganization::CALifeOrganization	(LPCSTR section)
{
	LPCSTR						S;
	string64					S1;
	S							= pSettings->r_string	(section,"discoveries");
	m_possible_discoveries.resize(_GetItemCount(S));
	LPSTR_IT					B = m_possible_discoveries.begin(), I = B;
	LPSTR_IT					E = m_possible_discoveries.end();
	for ( ; I != E; ++I) {
		_GetItem				(S,int(I - B),S1);
		*I						= (char*)xr_malloc((xr_strlen(S1) + 1)*sizeof(char));
		strcpy					((char*)(*I),S1);
	}
	m_join_probability			= pSettings->r_float	(section,"join_probability");
	m_left_probability			= pSettings->r_float	(section,"left_probability");
	m_id						= _ORGANIZATION_ID		(pSettings->r_u32(section,"trader_rank"));
	strcpy						(m_discovery_to_investigate,"");
	m_research_state			= eResearchStateLeft;
}

CALifeOrganization::~CALifeOrganization	()
{
	delete_data					(m_possible_discoveries);
	delete_data					(m_purchased_artefacts);
}

void CALifeOrganization::save			(IWriter &memory_stream)
{
	memory_stream.w				(&m_research_state,	sizeof(m_research_state));
	memory_stream.w				(&m_finish_time,	sizeof(m_finish_time));
	memory_stream.w_string		(m_discovery_to_investigate);
	{
		memory_stream.w_u32		(m_ordered_artefacts.size());
		ARTEFACT_ORDER_IT		I = m_ordered_artefacts.begin();
		ARTEFACT_ORDER_IT		E = m_ordered_artefacts.end();
		for ( ; I != E; ++I) {
			memory_stream.w_string((*I).m_section);
			memory_stream.w_u32	((*I).m_count);
			memory_stream.w_u32	((*I).m_price);
		}
	}
	{
		memory_stream.w_u32		(m_purchased_artefacts.size());
		ITEM_COUNT_PAIR_IT		I = m_purchased_artefacts.begin();
		ITEM_COUNT_PAIR_IT		E = m_purchased_artefacts.end();
		for ( ; I != E; ++I) {
			memory_stream.w_string((*I).first);
			memory_stream.w		(&((*I).second),sizeof((*I).second));
		}
	}
}

void CALifeOrganization::load			(IReader &file_stream)
{
	file_stream.r				(&m_research_state,	sizeof(m_research_state));
	file_stream.r				(&m_finish_time,	sizeof(m_finish_time));
	file_stream.r_string		(m_discovery_to_investigate);
	{
		m_ordered_artefacts.resize	(file_stream.r_u32());
		ARTEFACT_ORDER_IT		I = m_ordered_artefacts.begin();
		ARTEFACT_ORDER_IT		E = m_ordered_artefacts.end();
		for ( ; I != E; ++I) {
			file_stream.r_string((*I).m_section);
			(*I).m_count		= file_stream.r_u32();
			(*I).m_price		= file_stream.r_u32();
		}
	}
	{
		u32						count = file_stream.r_u32();
		for (u32 i=0; i<count; ++i) {
			string32			S;
			file_stream.r_string(S);
			LPSTR				section = (char*)xr_malloc(32*sizeof(char));
			strcpy				(section,S);
			m_purchased_artefacts.insert(mk_pair(section,file_stream.r_u32()));
		}
	}
}
