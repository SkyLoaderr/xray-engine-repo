////////////////////////////////////////////////////////////////////////////
//	Module 		: selector_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Selector manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_params_level_evaluator.h"

class CCustomMonster;

class CSelectorManager {
public:
	typedef CAbstractVertexEvaluator			evaluator;
	typedef xr_map<LPCSTR,evaluator*,pred_str>	SELECTORS;

protected:
	SELECTORS			m_selectors;
	CCustomMonster		*m_object;

public:
						CSelectorManager	();
	virtual				~CSelectorManager	();
	virtual	void		reinit				(CCustomMonster *object);
			void		init_selector		(evaluator &S);
	template <typename T> 
	IC		void		add					(LPCSTR section, LPCSTR selector_name);
			void		remove				(LPCSTR selector_name);
			void		remove_all			();
	IC		evaluator	*selector			(LPCSTR selector_name);
};

#include "selector_manager_inline.h"
