

#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingSleep
// Сон
//////////////////////////////////////////////////////////////////////////

class CStateBitingSleep : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingSleep	(LPCSTR state_name);
	virtual				~CStateBitingSleep	();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Biting *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

