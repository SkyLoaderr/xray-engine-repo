#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingWander
// Бродить по точкам графа
//////////////////////////////////////////////////////////////////////////

class CStateBitingWander : public CStateBase<CAI_Biting> {
protected:
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingWander	(LPCSTR state_name);
	virtual				~CStateBitingWander	();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Biting *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

