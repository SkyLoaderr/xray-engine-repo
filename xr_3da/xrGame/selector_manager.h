////////////////////////////////////////////////////////////////////////////
//	Module 		: selector_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Selector manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_level_selector.h"

class CSelectorManager {
protected:
	xr_map<LPCSTR,PathManagers::CAbstractVertexEvaluator*,pred_str>	m_selectors;
public:
					CSelectorManager		();
	virtual			~CSelectorManager		();
			void	Init					();
	virtual	void	reinit					();
			void	init_selector			(PathManagers::CAbstractVertexEvaluator &S);
	template <typename T> 
	IC		void	add						(LPCSTR section, LPCSTR selector_name);
			void	remove					(LPCSTR selector_name);
			void	remove_all				();
	IC		PathManagers::CAbstractVertexEvaluator	*selector(LPCSTR selector_name);
};

#include "selector_manager_inline.h"
