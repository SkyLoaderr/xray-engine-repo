// CustomItem.cpp: implementation of the CCustomItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\xr_mac.h"
#include "actor.h"
#include "CustomItem.h"

#define TIME_TO_SHOW	10.f
#define SHOWING_TIME	.5f
#define HIDDING_TIME	.07f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomItem::CCustomItem() : CObject()
{
	fTime			= Random.randF(PI_MUL_2);
	state			= es_forcedword;
	SetState		(esShowing);
}

CCustomItem::~CCustomItem()
{
	pSounds->Delete3D(sndTake);
}

void	CCustomItem::SetState(EItemState e)
{
	const float SC=.0001f;
	const float SM=1.f;
	if (e==state) return;
	state = e;
	switch(state){
	case esShowing:
		vScale.set(SC,SC,SC);
		UpdateTransform();
		bVisible = true;
		fStateTime = SHOWING_TIME; 
		break;
	case esShow:	
		bVisible = true;
		break;
	case esHidding:	
		pSounds->Play3DAtPos(sndTake,vPosition);
		vScale.set(SM,SM,SM);
		UpdateTransform();
		bVisible = true;
		fStateTime = HIDDING_TIME; 
		break;
	case esHide:	
		bVisible = false;
		fStateTime = TIME_TO_SHOW;
		break;
	}
}

void	CCustomItem::Load(CInifile *pIni, const char *section)
{
	CObject::Load	(pIni,section);
	
	clsid_target	= TEXT2CLSID(pIni->ReadSTRING(section, "target"));
	iValue			= pIni->ReadINT(section, "value");
	start_pos.set	(vPosition);
	pSounds->Create3D(sndTake, pIni->ReadSTRING(section, "snd_take"));
}

void	CCustomItem::OnMove()
{
	fTime+=Device.fTimeDelta*2.5f; //*.1f;
	vPosition.y	= start_pos.y +	sinf(fTime)*.1f;
	Fvector	D,N;
	D.set(-sinf(fTime),0.f,-cosf(fTime));
	N.set(0,1,0);
	mRotate.rotation(D,N);

	switch(state){
	case esShowing:
		{ 
			fStateTime -= Device.fTimeDelta;
			float   fS = 1.f-(fStateTime/SHOWING_TIME);
			vScale.set(fS,fS,fS);
			UpdateTransform();
			if (fStateTime<=0.f) SetState(esShow);
		}
		break;
	case esShow:	
		UpdateTransform	();
		break;
	case esHidding:	
		{
			fStateTime -= Device.fTimeDelta;
			float   fS = fStateTime/HIDDING_TIME;
			vScale.set(fS,fS,fS);
			UpdateTransform();
			if (fStateTime<=0.f) SetState(esHide);
		}
		break;
	case esHide:
		fStateTime-=Device.fTimeDelta;
		if (fStateTime<0) SetState(esShowing);
		break;
	}
}

void CCustomItem::OnNear	( CObject* O )
{
	if (O->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
	{
		if (state!=esShow) return;
		if (((CActor *)O)->TakeItem(pCreator->Objects.GetObjectCID(this))){
			SetState(esHidding);
		}
		return;
	}
}
