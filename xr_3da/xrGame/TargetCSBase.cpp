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
		setVisible					(true);
		setEnabled					(true);
	}

	return res;
}
void CTargetCSBase::OnDeviceCreate()
{
	CCF_Shape*	shape			= new CCF_Shape	(this);
	cfModel						= shape;
	Fsphere S;	S.P.set			(0,0,0); S.R = radius;
	shape->add_sphere			(S);

	shape->ComputeBounds						();
	pCreator->ObjectSpace.Object_Register		(this);
	cfModel->OnMove								();
}
