#include "stdafx.h"
#include "targetcsbase.h"
#include "targetcs.h"

CTargetCSBase::CTargetCSBase(void)
{
}

CTargetCSBase::~CTargetCSBase(void)
{
}
BOOL CTargetCSBase::net_Spawn		(LPVOID DC)
{
	BOOL res	= inherited::net_Spawn(DC);

	if (res){
		xrSE_Target_CSBase* T		= (xrSE_Target_CSBase*)DC;
		radius	= T->radius;
		team	= T->s_team;
		setVisible					(true);
		setEnabled					(true);
		Game().targets.push_back	(this);

		CCF_Shape*	shape			= xr_new<CCF_Shape>	(this);
		cfModel						= shape;
		Fsphere S;	S.P.set			(0,0,0); S.R = radius;
		shape->add_sphere			(S);

		shape->ComputeBounds						();
		pCreator->ObjectSpace.Object_Register		(this);
		cfModel->OnMove								();
	}

	return res;
}

void CTargetCSBase::net_Destroy	()
{
	inherited::net_Destroy		();
	Game().targets.erase(find(Game().targets.begin(), Game().targets.end(), this));
}

void CTargetCSBase::feel_touch_new			(CObject* O) {
	CTargetCS *l_pTarg = dynamic_cast<CTargetCS*>(O);
	if(l_pTarg) {
		l_pTarg=l_pTarg;
	}
}

void CTargetCSBase::feel_touch_delete		(CObject* O) {
}

void CTargetCSBase::Update(u32 dt)
{
	inherited::Update	(dt);

	feel_touch_update	(vPosition,radius);
}
