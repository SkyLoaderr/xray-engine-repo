////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_actions.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager actions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAgentManager;

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionBase
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionBase : public CActionBase<CAgentManager> {
protected:
	typedef CActionBase<CAgentManager>				inherited;
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::_solver_value_type		_value_type;

public:
						CAgentManagerActionBase		(CAgentManager *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		finalize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionNoOrders
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionNoOrders : public CAgentManagerActionBase {
protected:
	typedef CAgentManagerActionBase inherited;

public:
						CAgentManagerActionNoOrders	(CAgentManager *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		finalize					();
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionGatherItems
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionGatherItems : public CAgentManagerActionBase {
protected:
	typedef CAgentManagerActionBase inherited;

public:
						CAgentManagerActionGatherItems	(CAgentManager *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		finalize						();
	virtual void		execute							();
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionKillEnemy : public CAgentManagerActionBase {
protected:
	typedef CAgentManagerActionBase inherited;

	u32					m_level_time;

public:
						CAgentManagerActionKillEnemy(CAgentManager *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		finalize					();
	virtual void		execute						();
};

#include "agent_manager_actions_inline.h"