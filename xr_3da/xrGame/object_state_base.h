////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state.h
//	Created 	: 16.01.2004
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CObjectHandler;

class CObjectStateBase : public CStateBase<CObjectHandler> {
protected:
	typedef CStateBase<CObjectHandler> inherited;

private:
	u32					m_priority;

public:
						CObjectStateBase	();
	virtual				~CObjectStateBase	();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CObjectHandler *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

#include "state_free_no_alife_inline.h"