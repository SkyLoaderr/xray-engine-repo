////////////////////////////////////////////////////////////////////////////
//	Module 		: action_base.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Base action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_management_config.h"
#include "graph_engine.h"

template <typename _object_type>
class CActionBase : public CGraphEngine::CWorldOperator {
protected:
	typedef CGraphEngine::CWorldOperator	inherited;
	typedef CGraphEngine::CWorldProperty	COperatorCondition;

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
	_object_type			*m_object;
	u32						m_start_level_time;
	ALife::_TIME_ID			m_start_game_time;
	u32						m_inertia_time;
	bool					m_log;
	LPCSTR					m_action_name;

protected:
#ifdef LOG_ACTION
	virtual void			debug_log			(const EActionStates state_state) const;
#endif

public:
	IC						CActionBase			(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, _object_type *object = 0, LPCSTR action_name = "");
	IC						CActionBase			(_object_type *object = 0, LPCSTR action_name = "");
	virtual					~CActionBase		();
			void			init				(_object_type *object, LPCSTR action_name);
	virtual	void			Load				(LPCSTR section);
	virtual	void			reinit				(_object_type *object, bool clear_all);
	virtual	void			reload				(LPCSTR section);
	virtual void			initialize			();
	virtual void			execute				();
	virtual void			finalize			();
	IC		void			set_inertia_time	(u32 inertia_time);
	IC		u32				start_level_time	() const;
	IC		ALife::_TIME_ID	start_game_time		() const;
	IC		u32				inertia_time		() const;
	IC		bool			completed			() const;
};

#include "action_base_inline.h"