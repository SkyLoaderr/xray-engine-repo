#pragma once

#include "action_base.h"

class CAI_Stalker;

class CObjectActionBase : public CActionBase<CAI_Stalker> {
protected:
	typedef CActionBase<CAI_Stalker> inherited;

public:
						CObjectActionBase(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, LPCSTR action_name = "") :
							inherited	(conditions,effects,action_name)
	{
	}
};