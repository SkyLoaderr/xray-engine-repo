#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateBurerAttackTele : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

public:
						CStateBurerAttackTele	(LPCSTR state_name);
	virtual				~CStateBurerAttackTele	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "burer_state_attack_tele_inline.h"
