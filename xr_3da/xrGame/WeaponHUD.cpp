// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeaponHUD.h"
#include "Weapon.h"
#include "inventory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponHUD::CWeaponHUD()
{
	mTransform.identity			();
	m_Offset.identity			();
	pVisualName					= 0;
	iFireBone					= -1;
}

CWeaponHUD::~CWeaponHUD()
{
	::Render->model_Delete		(pVisual);
}

void CWeaponHUD::Load			(LPCSTR section)
{
	// Geometry and transform
	Fvector						pos,ypr;
	pos							= pSettings->r_fvector3(section,"position");
	ypr							= pSettings->r_fvector3(section,"orientation");
	ypr.mul						(PI/180.f);

	m_Offset.setHPB				(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over		(pos);

	// Visual
	pVisualName					= pSettings->r_string(section, "visual");
	pVisual						= ::Render->model_Create(*pVisualName);
	R_ASSERT					(pVisual->Type==MT_SKELETON);

	// fire bone	
	LPCSTR fire_bone			= pSettings->r_string					(section,"fire_bone");
	iFireBone					= PKinematics(Visual())->LL_BoneID	(fire_bone);
	if (iFireBone<0)			Debug.fatal	("There is no 'fire_bone' for weapon '%s'.",section);
	vFirePoint					= pSettings->r_fvector3					(section,"fire_point");
	if(pSettings->line_exist(section,"fire_point2")) vFirePoint2 = pSettings->r_fvector3(section,"fire_point2");
	else vFirePoint2 = vFirePoint;
	vShellPoint					= pSettings->r_fvector3					(section,"shell_point");

	// play default animation
	pVisual						= ::Render->model_Create(*pVisualName);
	// PKinematics				(pVisual)->PlayCycle("idle");
}

void CWeaponHUD::UpdatePosition(const Fmatrix& trans)
{
	mTransform.mul	(trans,m_Offset);
}

CMotionDef* CWeaponHUD::animGet		(LPCSTR name)
{
	return PKinematics(Visual())->ID_Cycle_Safe(name);
}

static void __stdcall animCallback	(CBlend* B)
{
	//CWeapon* W			= (CWeapon*)B->CallbackParam;
	//W->OnAnimationEnd	();
	PIItem(B->CallbackParam)->OnAnimationEnd();
}

void CWeaponHUD::animPlay			(CMotionDef* M,	BOOL bMixIn, CInventoryItem* W)
{
	PKinematics(pVisual)->Update			();
	if (W)	PKinematics(pVisual)->PlayCycle	(M,bMixIn,animCallback,W);
	else	PKinematics(pVisual)->PlayCycle	(M,bMixIn);
	PKinematics(pVisual)->Invalidate		();
}
