#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingEatPrepareDrag
// тянуть труп
//////////////////////////////////////////////////////////////////////////

class CStateBitingEatPrepreDrag : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingEatPrepreDrag	(LPCSTR state_name);
	virtual				~CStateBitingEatPrepreDrag	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

	virtual	bool		completed				() const;
};


//////////////////////////////////////////////////////////////////////////
// CStateBitingEatDrag
// тянуть труп
//////////////////////////////////////////////////////////////////////////

class CStateBitingEatDrag : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingEatDrag		(LPCSTR state_name);
	virtual				~CStateBitingEatDrag	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

	virtual	bool		completed				() const;
};


