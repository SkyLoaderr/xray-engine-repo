#pragma once
#ifndef CPHOBJECT
#define CPHOBJECT


class CPHObject {
xr_list<CPHObject*>::iterator m_ident;
public:
	virtual void PhDataUpdate(dReal step)=0;
	virtual void PhTune(dReal step)=0;
	virtual void InitContact(dContact* c)=0;
	virtual void StepFrameUpdate(dReal step)=0;

	void		 Activate();
	void		 Deactivate();
};
#endif//CPHOBJECT
