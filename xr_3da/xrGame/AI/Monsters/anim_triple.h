#pragma once

#include "control_combase.h"
#include "../../../SkeletonAnimated.h"

enum EStateAnimTriple {
	eStatePrepare,
	eStateExecute,
	eStateFinalize,
	eStateNone
};

struct STripleAnimEventData : public ControlCom::IEventData {
	u32		m_current_state;
	IC		STripleAnimEventData(u32 state) : m_current_state(state) {}
};

struct SAnimationTripleData : public ControlCom::IComData {
	MotionID	pool[3];
	bool		skip_prepare;
	bool		execute_once;
};

class CAnimationTriple : public CControl_ComCustom<SAnimationTripleData>{
	EStateAnimTriple		m_current_state;
	EStateAnimTriple		m_previous_state;
public:
	virtual	void	on_capture				();
	virtual void	on_release				();
	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual bool	check_start_conditions	();
	virtual void	activate				();

			void	pointbreak				();
private:
			void	select_next_state		();
			void	play_selected			();
};

