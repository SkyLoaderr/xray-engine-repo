#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateBurerAttackGraviStart : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

	u32					attack_started; 
	bool				fade_in;

public:
						CStateBurerAttackGraviStart	(LPCSTR state_name);
	virtual				~CStateBurerAttackGraviStart();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

};

template<typename _Object>
class CStateBurerAttackGraviFinish : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

	u32					attack_started; 
	bool				fade_in;

public:
						CStateBurerAttackGraviFinish	(LPCSTR state_name);
	virtual				~CStateBurerAttackGraviFinish	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "burer_state_attack_gravi_inline.h"
