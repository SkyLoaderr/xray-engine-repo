////////////////////////////////////////////////////////////////////////////
//	Module 		: setup_action.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : setup action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "animation_action.h"
#include "movement_action.h"
#include "object_action.h"
#include "sight_action.h"
#include "sound_action.h"

class CAI_Stalker;

class CSetupAction {
protected:
	// action params
	CAI_Stalker			*m_object;
	u32					m_start_time;
	u32					m_inertia_time;
	float				m_weight;
	// sound params
	CAnimationAction	m_animation_action;
	CMovementAction		m_movement_action;
	CObjectAction		m_object_action;
	CSightAction		m_sight_action;
	CSoundAction		m_sound_action;

public:
	IC						CSetupAction		(float weight, u32 inertia_time);
	IC		void			set_object			(CAI_Stalker *object);
	virtual	bool			applicable			() const;
	virtual bool			completed			() const;
	virtual void			initialize			();
	virtual void			execute				();
	virtual void			finalize			();
	virtual float			weight				() const;
	IC		CSetupAction	&animation			(const CAnimationAction &animation_action);
	IC		CSetupAction	&movement			(const CMovementAction &movement_action);
	IC		CSetupAction	&object				(const CObjectAction &object_action);
	IC		CSetupAction	&sight				(const CSightAction	&sight_action);
	IC		CSetupAction	&sound				(const CSoundAction	&sound_action);
};

#include "setup_action_inline.h"