#include "stdafx.h"
#include "CustomDoor.h"
#include "..\bodyinstance.h"
#include "..\fmesh.h"
#include "..\3dsound.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCustomDoor::CCustomDoor()
{
	state		= edsClose;
}

CCustomDoor::~CCustomDoor()
{	
	pSounds->Delete3D(sndOpenClose);
}

void CCustomDoor::Load(CInifile* ini, const char * section)
{
	inherited::Load				(ini,section);

	R_ASSERT	(pVisual->Type==MT_SKELETON);
	PKinematics(pVisual)->PlayCycle("close");

	if (ini->LineExists(section,"sound"))
	{
		LPCSTR N	= ini->ReadSTRING(section,"sound");
		pSounds->Create3D(sndOpenClose,N);
	}
}

void CCustomDoor::Open(){
	PKinematics(pVisual)->PlayCycle("open");
	Fsphere& S = CFORM()->GetSphere();
	pSounds->Play3DAtPos(sndOpenClose,S.P);
}

void CCustomDoor::Close(){
	PKinematics(pVisual)->PlayCycle("close");
	Fsphere& S = CFORM()->GetSphere();
	pSounds->Play3DAtPos(sndOpenClose,S.P);
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
		OnEnter.Signal((DWORD)O);
		Contacted.push_back(O);
		return;
	}
*/	
	return;
}
