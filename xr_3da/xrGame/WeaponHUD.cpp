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

	Fquaternion					Q;
	Q.rotationYawPitchRoll		(ypr.y,ypr.x,ypr.z);
	m_Offset.rotation			(Q);
	m_Offset.translate_over		(pos);

	// Visual
	pVisualName					= strdup(ini->ReadSTRING(section, "visual"));
	pVisual						= ::Render.Models.Create(pVisualName);
	R_ASSERT					(pVisual->Type==MT_SKELETON);
	PKinematics					(pVisual)->PlayCycle("idle");
}

void CWeaponHUD::UpdatePosition(const Fmatrix& trans){
	mTransform.mul	(trans,m_Offset);
}

void CWeaponHUD::UpdateAnimation()
{
// update animation
	if (cur_mstate!=new_mstate){
		switch(new_mstate){
		case hsIdle: PKinematics(pVisual)->PlayCycle("idle"); break;
		case hsFire: PKinematics(pVisual)->PlayCycle("fire"); break;
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

