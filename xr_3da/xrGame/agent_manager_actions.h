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
// CAgentManagerActionIdle
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionIdle : public CAgentManagerActionBase {
protected:
	typedef CAgentManagerActionBase inherited;

public:
						CAgentManagerActionIdle		(CAgentManager *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		finalize					();
	virtual void		execute						();
};

#include "agent_manager_actions_inline.h"