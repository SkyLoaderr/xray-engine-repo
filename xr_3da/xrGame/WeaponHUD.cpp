// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\render.h"
#include "..\fmesh.h"
#include "..\bodyinstance.h"
#include "WeaponHUD.h"
#include "Weapon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponHUD::CWeaponHUD()
{
	mTransform.identity			();
	m_Offset.identity			();
	pVisualName					= 0;
	pVisual						= 0;
	iFireBone					= -1;

	Device.seqDevDestroy.Add	(this);
	Device.seqDevCreate.Add		(this);
}

CWeaponHUD::~CWeaponHUD()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);

	_FREE						(pVisualName);
	::Render->model_Delete		(pVisual);
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
	pVisual						= ::Render->model_Create(pVisualName);
	R_ASSERT					(pVisual->Type==MT_SKELETON);

	// fire bone	
	LPCSTR fire_bone			= ini->ReadSTRING					(section,"fire_bone");
	iFireBone					= PKinematics(Visual())->LL_BoneID	(fire_bone);
	if (iFireBone<0)			Device.Fatal("There is no 'fire_bone' for weapon '%s'.",section);
	vFirePoint					= ini->ReadVECTOR					(section,"fire_point");
	vShellPoint					= ini->ReadVECTOR					(section,"shell_point");

	// play default animation
	// PKinematics				(pVisual)->PlayCycle("idle");
}

void CWeaponHUD::UpdatePosition(const Fmatrix& trans)
{
	mTransform.mul	(trans,m_Offset);
}

void CWeaponHUD::OnDeviceDestroy	()
{
	::Render->model_Delete		(pVisual);
}
void CWeaponHUD::OnDeviceCreate	()
{
	pVisual						= ::Render->model_Create(pVisualName);
}
CMotionDef* CWeaponHUD::animGet		(LPCSTR name)
{
	return PKinematics(Visual())->ID_Cycle_Safe(name);
}

static void __stdcall animCallback	(CBlend* B)
{
	CWeapon* W			= (CWeapon*)B->CallbackParam;
	W->OnAnimationEnd	();
}

void CWeaponHUD::animPlay			(CMotionDef* M,	BOOL bMixIn, CWeapon* W)
{
	if (W)	PKinematics(pVisual)->PlayCycle(M,bMixIn,animCallback,W);
	else	PKinematics(pVisual)->PlayCycle(M,bMixIn);
	PKinematics(pVisual)->Invalidate();
}
