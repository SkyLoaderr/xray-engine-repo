// MonsterAlien.cpp: implementation of the CMonsterAlien class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\..\xr_creator.h"
#include "..\..\customactor.h"
#include "..\..\x_ray.h"
#include "..\..\xr_smallfont.h"
#include "MonsterAlien.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAlien::CMonsterAlien(void *p) : CObject()
{
}

CMonsterAlien::~CMonsterAlien()
{

}

void CMonsterAlien::OnMove()
{
	Eye.dir.sub			(pCreator->GetRespawnPoint(),vPosition);
	Eye.dir.normalize	();

	CCustomActor* A = pCreator->GetPlayer();
	if (pCreator->AI.CanISee(Eye,A->Position(),A->Radius()))
	{
		pApp->pFont->Out(-0.5f,0,"I see you!!!");
	}

	Fvector tmp;
	Navigator.SetTarget		(&(A->Position()));
	Navigator.OnMove		();
	Navigator.GetOrientation(vPosition,tmp);
	UpdateTransform	();
}

void CMonsterAlien::Load(CInifile* ini, const char* section)
{
	CObject::Load(ini,section);

	// Eyes
	Eye.set_fov		(pSettings->ReadFLOAT("m_alien","eye_fov"));
	Eye.set_sight	(pSettings->ReadFLOAT("m_alien","eye_distance"));
	Eye.from.set	(vPosition);

	// Navigation
	Navigator.Reset(vPosition,0,3.f);
}
