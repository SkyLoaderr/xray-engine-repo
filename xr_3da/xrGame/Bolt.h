#pragma once
#include "missile.h"

class CBolt :
	public CMissile
{
	typedef CMissile inherited;
public:
	CBolt(void);
	virtual ~CBolt(void);

	virtual bool Activate();
	virtual void Deactivate();
	virtual bool Attach(PIItem pIItem, bool force = false);
	virtual void Throw();
};
