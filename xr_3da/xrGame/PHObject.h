#pragma once
#ifndef CPHOBJECT
#define CPHOBJECT


class CPHObject {
xr_list<CPHObject*>::iterator m_ident;
bool		b_activated;
public:
	virtual void PhDataUpdate	(dReal step)							=0;
	virtual void PhTune			(dReal step)							=0;
	virtual void InitContact	(dContact* c,bool& do_collide)			=0;
	virtual void Freeze			()										=0;
	virtual void UnFreeze		()										=0;
	//virtual void StepFrameUpdate(dReal step)=0;
					CPHObject()	;
	void			Activate()	;
	void			Deactivate();
};
#endif//CPHOBJECT