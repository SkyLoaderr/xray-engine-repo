////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_in_direction_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger in direction actions classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionTakeCover : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionDangerInDirectionTakeCover	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize									();
	virtual void		execute										();
	virtual void		finalize									();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionLookOut : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionDangerInDirectionLookOut		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize									();
	virtual void		execute										();
	virtual void		finalize									();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionLookAround
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionLookAround : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionDangerInDirectionLookAround	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize									();
	virtual void		execute										();
	virtual void		finalize									();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionSearch
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionSearch : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
						CStalkerActionDangerInDirectionSearch	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize								();
	virtual void		execute									();
	virtual void		finalize								();
};
