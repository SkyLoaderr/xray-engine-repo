////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_task_actions.h
//	Created 	: 25.10.2004
//  Modified 	: 25.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife task action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

class CAI_Trader;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionReachTaskLocation
//////////////////////////////////////////////////////////////////////////

class CStalkerActionReachTaskLocation : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionReachTaskLocation	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAccomplishTask
//////////////////////////////////////////////////////////////////////////

class CStalkerActionAccomplishTask : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

protected:
	float				m_max_search_distance;

public:
						CStalkerActionAccomplishTask(CAI_Stalker *object, LPCSTR action_name = "");
	virtual	void		reinit						(CAI_Stalker *object, CPropertyStorage *storage, bool clear_all);
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionReachCustomerLocation
//////////////////////////////////////////////////////////////////////////

class CStalkerActionReachCustomerLocation : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionReachCustomerLocation(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFollowCustomer
//////////////////////////////////////////////////////////////////////////

class CStalkerActionCommunicateWithCustomer : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

private:
	CAI_Trader			*m_trader;

public:
						CStalkerActionCommunicateWithCustomer(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSolveZonePuzzle
//////////////////////////////////////////////////////////////////////////

class CStalkerActionSolveZonePuzzle : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

protected:
	u32					m_stop_weapon_handling_time;

public:
						CStalkerActionSolveZonePuzzle	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};
