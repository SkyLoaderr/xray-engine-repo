////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_manager.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_abstract.h"

template <
	template <typename _object_type> _motivation_type,
	typename _object_type
>
class CMotivationManager {
public:
	typedef _motivation_type<_object_type>	CMotivation;
	typedef CGraphAbstract<
		CMotivation,
		float,
		u32,
		u16
	> CSGraphAbstract;

protected:
	CSGraphAbstract					*m_graph;
	xr_vector<u32>					m_motivations;
	xr_map<u32,float>				m_actions;
	u32								m_selected_id;
	bool							m_actuality;
	xr_set<u32>						m_temp;
	_object_type					*m_object;			

protected:
	IC		CSGraphAbstract			&graph				();
	IC		void					update_motivations	();
	IC		void					select_action		();
	IC		void					propagate			(u32 motivation_id, float weight);

public:
	IC								CMotivationManager	();
	virtual							~CMotivationManager	();
	IC		void					init				();
	virtual void					reinit				(_object_type *object, bool clear_all = false);
	virtual void					Load				(LPCSTR section);
	virtual void					reload				(LPCSTR section);
	virtual void					update				(u32 time_delta);

public:
	IC		void					add_motivation		(u32 motivation_id, CMotivation *motivation);
	IC		void					remove_motivation	(u32 motivation_id);
	IC		void					add_connection		(u32 motivation_id, u32 sub_motivation_id);
	IC		void					remove_connection	(u32 motivation_id, u32 sub_motivation_id);
	IC		const CMotivation		*motivation			(u32 motivation_id) const;
	IC		const CMotivation		*selected			() const;
	IC		u32						selected_id			() const;
	IC		const CSGraphAbstract	&graph				() const;
	IC		bool					actual				() const;
};

#include "motivation_manager_inline.h"
