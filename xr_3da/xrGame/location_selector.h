////////////////////////////////////////////////////////////////////////////
//	Module 		: location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_selector.h"
#include "gameobject.h"

class CLocationSelector : virtual public CGameObject {
protected:
	struct SEnemySelected
	{
		CEntity*			Enemy;
		bool				bVisible;
		float				fCost;
	};

	Fvector					m_tHitDirection;
	u32						m_dwHitTime;
	u32						m_dwCurrentUpdate;
	SEnemySelected			m_tSelectedEnemy;
	u32						m_dwLastLocationSelectionTime;
public:
					CLocationSelector		();
	virtual			~CLocationSelector		();
	virtual void	init_evaluator			(PathManagers::CAbstractNodeEvaluator *node_evaluator);
			void	select_location			(PathManagers::CAbstractNodeEvaluator *node_evaluator);
};