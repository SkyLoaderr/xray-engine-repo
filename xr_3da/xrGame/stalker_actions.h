////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_script_base.h"

class CAI_Stalker;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionBase
//////////////////////////////////////////////////////////////////////////

class CStalkerActionBase : public CActionScriptBase<CAI_Stalker> {
protected:
	typedef CActionScriptBase<CAI_Stalker>			inherited;
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::_solver_value_type		_value_type;

public:
						CStalkerActionBase	(CAI_Stalker *object, LPCSTR action_name = "");
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDead : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionDead	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		execute				();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFreeNoALife
//////////////////////////////////////////////////////////////////////////

class CStalkerActionFreeNoALife : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionFreeNoALife	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		execute						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGatherItems
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGatherItems : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGatherItems	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemy : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionKillEnemy		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetKillDistance
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetKillDistance : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetKillDistance	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKill : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetReadyToKill(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetEnemy : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
								CStalkerActionGetEnemy	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void				initialize				();
	virtual void				execute					();
	virtual void				finalize				();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemySeen
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetEnemySeen : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetEnemySeen	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetItemToKill : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionGetItemToKill	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFindItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionFindItemToKill : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionFindItemToKill(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionMakeItemKilling
//////////////////////////////////////////////////////////////////////////

class CStalkerActionMakeItemKilling : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionMakeItemKilling	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFindAmmo
//////////////////////////////////////////////////////////////////////////

class CStalkerActionFindAmmo : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
								CStalkerActionFindAmmo	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void				initialize				();
	virtual void				execute					();
	virtual void				finalize				();
};

#include "stalker_actions_inline.h"