// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\fstaticrender.h"
#include "..\fmesh.h"
#include "..\bodyinstance.h"
#include "WeaponHUD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponHUD::CWeaponHUD()
{
	cur_mstate	= hsIdle;
	new_mstate	= hsIdle;
	mTransform.identity			();
	m_Offset.identity			();
	pVisualName	= 0;
	pVisual		= 0;
	iFireBone	= -1;

	Device.seqDevDestroy.Add	(this);
	Device.seqDevCreate.Add		(this);
}

CWeaponHUD::~CWeaponHUD()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);

	_FREE						(pVisualName);
	::Render.Models.Delete		(pVisual);
}

void CWeaponHUD::Load(CInifile* ini, const char* section)
{
	// Geometry and transform
	Fvector						pos,ypr;
	pos							= ini->ReadVECTOR(section,"position");
	ypr							= ini->ReadVECTOR(section,"orientation");
	ypr.mul						(PI/180.f);

	m_Offset.setHPB				(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over		(pos);

	// Visual
	pVisualName					= strdup(ini->ReadSTRING(section, "visual"));
	pVisual						= ::Render.Models.Create(pVisualName);
	R_ASSERT					(pVisual->Type==MT_SKELETON);

	// fire bone	
	LPCSTR fire_bone			= ini->ReadSTRING	(section,"fire_bone");
	iFireBone					= PKinematics(Visual())->LL_BoneID(fire_bone);
	vFirePoint					= ini->ReadVECTOR(section,"fire_point");

	// init anims
	mIdle						= PKinematics(Visual())->ID_Cycle("idle");
	mFireCycled					= PKinematics(Visual())->ID_Cycle_Safe("fire");
	mFireSpinup					= PKinematics(Visual())->ID_Cycle_Safe("spinup");
	char name[64];
	for (int i=0; ; i++){
		sprintf(name,"shoot%d",i);
		CMotionDef* M			= PKinematics(Visual())->ID_FX_Safe(name);
		if (!M)					break;
		mShoots.push_back		(M);
	}
	
	// play default animation
	PKinematics					(pVisual)->PlayCycle(mIdle);
}

void CWeaponHUD::UpdatePosition(const Fmatrix& trans){
	mTransform.mul	(trans,m_Offset);
}

void CWeaponHUD::Shoot(){ 
	VERIFY			(mShoots.size());
	new_mstate		= hsShoot; 
	int sh_id		= Random.randI(mShoots.size());
	PKinematics(pVisual)->PlayFX(mShoots[sh_id]);
}

void CWeaponHUD::UpdateAnimation()
{
// update animation
	if (cur_mstate!=new_mstate){
		switch(new_mstate){
		case hsIdle:		PKinematics(pVisual)->PlayCycle(mIdle);			break;
		case hsFireSpinup:	if (mFireSpinup) PKinematics(pVisual)->PlayCycle(mFireSpinup);	break;
		case hsFireCycle:	if (mFireCycled) PKinematics(pVisual)->PlayCycle(mFireCycled);	break;
		}
		cur_mstate = new_mstate;
	}
}

void CWeaponHUD::OnDeviceDestroy	()
{
	::Render.Models.Delete		(pVisual);
}
void CWeaponHUD::OnDeviceCreate	()
{
	pVisual						= ::Render.Models.Create(pVisualName);
}

