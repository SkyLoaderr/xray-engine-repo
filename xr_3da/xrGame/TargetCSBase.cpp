#include "stdafx.h"
#include "targetcsbase.h"

CTargetCSBase::CTargetCSBase(void)
{
}

CTargetCSBase::~CTargetCSBase(void)
{
}
BOOL CTargetCSBase::net_Spawn			(LPVOID DC)
{
	BOOL res	= inherited::net_Spawn(DC);

	if (res){
		xrSE_Target_CSBase* T		= (xrSE_Target_CSBase*)DC;
		radius	= T->radius;
		team	= T->s_team;
	}

	return res;
}
void CTargetCSBase::OnDeviceCreate()
{
	CCF_Shape*	shape			= new CCF_Shape	(this);
	cfModel						= shape;
	Fsphere S;	S.set			(Position(),radius);
	shape->add_sphere			(S);
}
