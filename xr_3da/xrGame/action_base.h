////////////////////////////////////////////////////////////////////////////
//	Module 		: action_base.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Base action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_management_config.h"

template <typename _object_type>
class CActionBase : public CGraphEngine::COperator {
protected:
	typedef CGraphEngine::COperatorCondition	COperatorCondition;

protected:
	enum EActionStates {
		eActionStateConstructed		= u32(0),
		eActionStateLoaded,
		eActionStateReinitialized,
		eActionStateReloaded,
		eActionStateInitialized,
		eActionStateExecuted,
		eActionStateFinalized,
		eActionStateDummy			= u32(-1),
	};

protected:
	u32						m_priority;
	_object_type			*m_object;
	u32						m_start_level_time;
	ALife::_TIME_ID			m_start_game_time;
	u32						m_inertia_time;
	bool					m_log;
	LPCSTR					m_action_name;

protected:
#ifdef LOG_STATE
	virtual void			debug_log			(const EActionStates state_state) const;
#endif

public:
	IC						CActionBase			(u32 m_priority, const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, LPCSTR action_name = "");
	virtual					~CActionBase		();
			void			init				(LPCSTR action_name);
	virtual	void			Load				(LPCSTR section);
	virtual	void			reinit				(_object_type *object);
	virtual	void			reload				(LPCSTR section);
	virtual void			initialize			();
	virtual void			execute				();
	virtual void			finalize			();
	virtual bool			completed			() const;
	IC		void			set_inertia_time	(u32 inertia_time);
	IC		u32				priority			() const;
	IC		u32				start_level_time	() const;
	IC		ALife::_TIME_ID	start_game_time		() const;
	IC		u32				inertia_time		() const;
};

#include "action_base_inline.h"