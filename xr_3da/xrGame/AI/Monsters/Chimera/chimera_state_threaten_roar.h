#pragma once
#include "../state.h"

template<typename _Object>
class CStateChimeraThreatenRoar : public CState<_Object> {
	typedef CState<_Object>		inherited;

public:
	IC					CStateChimeraThreatenRoar	(_Object *obj) : inherited(obj){}
	
	virtual	void		initialize					();	
	virtual	void		execute						();
	virtual bool		check_completion			();
};

#include "chimera_state_threaten_roar_inline.h"
