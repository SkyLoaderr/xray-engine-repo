////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_location_selector.h"
#include "game_graph.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class 
	CBaseLocationSelector<
		CGameGraph,
		typename _VertexEvaluator,
		typename _vertex_id_type
	> :
	public CAbstractLocationSelector <
		CGameGraph,
		typename _VertexEvaluator,
		typename _vertex_id_type
	>
{
	typedef CAbstractLocationSelector <
		CGameGraph,
		typename _VertexEvaluator,
		typename _vertex_id_type
	> inherited;
public:
	enum ESelectionType {
		eSelectionTypeMask				= u32(1),
		eSelectionTypeRandomBranching	= u32(1) << 1,
		eSelectionTypeDummy				= u32(-1),
	};

private:
	ESelectionType			m_selection_type;
public:
	IC						CBaseLocationSelector	();
	IC	virtual				~CBaseLocationSelector	();
	IC	virtual void		Init					(_Graph *graph = 0);
	IC				void	set_selection_type		(const ESelectionType selection_type);
	IC				void	get_selection_type		() const;
};

#include "game_location_selector_inline.h"
