// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeaponHUD.h"
#include "Weapon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponHUD::CWeaponHUD		()
{
	mTransform.identity			();
	m_Offset.identity			();
	pVisualName					= 0;
	iFireBone					= -1;
	m_pParentWeapon				= NULL;
	m_bHidden = true;
}
CWeaponHUD::CWeaponHUD(CHudItem* pHudItem)
{
	mTransform.identity			();
	m_Offset.identity			();
	pVisualName					= 0;
	iFireBone					= -1;
	
	m_pParentWeapon				= pHudItem;

	m_bHidden = true;
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
	R_ASSERT					(pVisual->Type==MT_SKELETON_ANIM);

	// fire bone	
	if(dynamic_cast<CWeapon*>(m_pParentWeapon))
	{
		LPCSTR fire_bone			= pSettings->r_string					(section,"fire_bone");
		iFireBone					= PKinematics(Visual())->LL_BoneID	(fire_bone);
		if (iFireBone<0)			Debug.fatal	("There is no 'fire_bone' for weapon '%s'.",section);
		vFirePoint					= pSettings->r_fvector3					(section,"fire_point");
	}
	else
	{
		iFireBone = -1;
	}
	
	if(pSettings->line_exist(section,"fire_point2")) 
		vFirePoint2 = pSettings->r_fvector3(section,"fire_point2");
	else 
		vFirePoint2 = vFirePoint;

	if(m_pParentWeapon && pSettings->line_exist(m_pParentWeapon->cNameSect(), "shell_particles")) 
	{
		vShellPoint = pSettings->r_fvector3	(section,"shell_point");
		//vShellDir	= pSettings->r_fvector3	(section,"shell_dir");
	}

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
	return PSkeletonAnimated(Visual())->ID_Cycle_Safe(name);
}

static void __stdcall animCallback	(CBlend* B)
{
	//CWeapon* W			= (CWeapon*)B->CallbackParam;
	//W->OnAnimationEnd	();
	PIItem(B->CallbackParam)->OnAnimationEnd();
}

void CWeaponHUD::animPlay			(CMotionDef* M,	BOOL bMixIn, CInventoryItem* W)
{
	Show	();

	// Msg	("- %s",PSkeletonAnimated(pVisual)->LL_MotionDefName_dbg(M));
	PSkeletonAnimated(pVisual)->Update		();
	if (W)	PSkeletonAnimated(pVisual)->PlayCycle	(M,bMixIn,animCallback,W);
	else	PSkeletonAnimated(pVisual)->PlayCycle	(M,bMixIn);
	PSkeletonAnimated(pVisual)->Invalidate	();
}
