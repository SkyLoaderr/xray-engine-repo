#pragma once
#ifndef CPHOBJECT
#define CPHOBJECT
class CPHObject {
public:
	virtual void PhDataUpdate(dReal step)=0;
	virtual void PhTune(dReal step)=0;
	virtual void InitContact(dContact* c)=0;
};
#endif//CPHOBJECT
