#include "stdafx.h"
#include "CustomDoor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCustomDoor::CCustomDoor()
{
	state		= edsClose;
}

CCustomDoor::~CCustomDoor()
{	
	::Sound->Delete(sndOpenClose);
}

void CCustomDoor::Load			(LPCSTR section)
{
	inherited::Load				(section);

	R_ASSERT	(pVisual->Type==MT_SKELETON);
	PKinematics(pVisual)->PlayCycle("close");

	if (pSettings->line_exist(section,"sound"))
	{
		LPCSTR N	= pSettings->r_string(section,"sound");
		::Sound->Create(sndOpenClose,TRUE,N);
	}
}

void CCustomDoor::Open(){
	PKinematics(pVisual)->PlayCycle("open");
	const Fsphere& S = CFORM()->getSphere();
	::Sound->PlayAtPos(sndOpenClose,this,S.P);
}

void CCustomDoor::Close(){
	PKinematics(pVisual)->PlayCycle("close");
	const Fsphere& S = CFORM()->getSphere();
	::Sound->PlayAtPos(sndOpenClose,this,S.P);
}

void CCustomDoor::OnMove()
{
//	PKinematics(pVisual)->Calculate();
//	PKinematics(pVisual)->Update();
	switch (state){
	case edsOpen:		break;
	case edsClose:		break;
	case edsOpening:	break;
	case edsClosing:	break;
	};
}

void CCustomDoor::OnNear( CObject* O )
{
/*
	// check if target
	if (O->SUB_CLS_ID != clsid_Target) return;
	
	// check if not contacted before
	if (find(Contacted.begin(),Contacted.end(),O)!=Contacted.end()) return;
	
	// check if it is actually contacted
	CCF_EventBox* M = (CCF_EventBox*)CFORM(); R_ASSERT	(M);
	if (M->Contact(O)) {
		OnEnter.Signal((u32)O);
		Contacted.push_back(O);
		return;
	}
*/	
	return;
}
