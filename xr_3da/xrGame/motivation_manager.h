////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_manager.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_abstract.h"

template <typename _motivation_type>
class CMotivationManager {
public:
	typedef CGraphAbstract<
		_motivation_type,
		float,
		u32,
		u16
	> CSGraphAbstract;

protected:
	CSGraphAbstract					*m_graph;
	xr_vector<u32>					m_motivations;
	xr_map<u32,float>				m_actions;
	u32								m_selected_id;

public:
	IC								CMotivationManager	();
	virtual							~CMotivationManager	();
	IC		void					init				();
	virtual void					reinit				();
	virtual void					Load				(LPCSTR section);
	virtual void					reload				(LPCSTR section);
	virtual void					update				(u32 time_delta);

	IC		void					add_motivation		(u32 motivation_id, _motivation_type *motivation);
	IC		void					remove_motivation	(u32 motivation_id);
	IC		void					add_connection		(u32 motivation_id, u32 sub_motivation_id);
	IC		void					remove_connection	(u32 motivation_id, u32 sub_motivation_id);
	IC		const _motivation_type	*motivation			(u32 motivation_id) const;
	IC		const _motivation_type	*selected			() const;
	IC		u32						selected_id			() const;
};

#include "motivation_manager_inline.h"
