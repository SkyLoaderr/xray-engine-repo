////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CGameGraph;
class CGameLevelCrossTable;
class CLevelGraph;
class CGraphEngine;
class CEF_Storage;
class CSE_ALifeSimulator;

class CAI_Space {
	CGameGraph				*m_game_graph;
	CGameLevelCrossTable	*m_cross_table;
	CLevelGraph				*m_level_graph;
	CGraphEngine			*m_graph_engine;
	CEF_Storage				*m_ef_storage;
	CSE_ALifeSimulator		*m_alife_simulator;
	IWriter					*m_lua_output;

			void						unload			();
	IC		void						set_alife		(CSE_ALifeSimulator *alife_simulator);
			friend class CSE_ALifeSimulator;
public:
										CAI_Space		();
	virtual								~CAI_Space		();
			void						load			(LPCSTR				level_name);
	IC		CGameGraph					&game_graph		() const;
	IC		CGameGraph					*get_game_graph	() const;
	IC		CLevelGraph					&level_graph	() const;
	IC		const CLevelGraph			*get_level_graph() const;
	IC		const CGameLevelCrossTable	&cross_table	() const;
	IC		const CGameLevelCrossTable	*get_cross_table() const;
	IC		CEF_Storage					&ef_storage		() const;
	IC		CGraphEngine				&graph_engine	() const;
	IC		const CSE_ALifeSimulator	&alife			() const;
	IC		const CSE_ALifeSimulator	*get_alife		() const;
	IC		IWriter						&lua_output		() const;
#ifdef DEBUG
			void						validate		(const u32			level_id) const;
#endif
};

#include "ai_space_inline.h"

extern CAI_Space *g_ai_space;

IC CAI_Space &ai()
{
	return			(*g_ai_space);
}