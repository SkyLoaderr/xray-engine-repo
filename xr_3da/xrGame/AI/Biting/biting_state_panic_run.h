
#pragma once
#include "../../state_base.h"


class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingPanicRun
// Бежать в панике
//////////////////////////////////////////////////////////////////////////

class CStateBitingPanicRun : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingPanicRun	(LPCSTR state_name);
	virtual				~CStateBitingPanicRun	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Biting *object);
	virtual	void		reload					(LPCSTR section);
	
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

