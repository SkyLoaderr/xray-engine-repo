// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\bodyinstance.h"
#include "..\fstaticrender.h"
#include "Weapon.h"
#include "WeaponHUD.h"

#include "entity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeapon::CWeapon(LPCSTR name)
{
	fTimeToFire	= 0;
	iHitPower	= 0;
	m_pParent	= 0;
	bVisible	= true;
	SetDefaults	();
	m_pHUD		= new CWeaponHUD();
	m_WpnName	= strupr(strdup(name));
	m_pContainer= 0;
}

CWeapon::~CWeapon()
{
	_FREE		(m_WpnName);
	_DELETE		(pVisual);
	_DELETE		(m_pHUD);
}

void CWeapon::SetParent(CEntity* parent, CWeaponList* container)
{
	R_ASSERT(parent);		m_pParent		= parent;
	R_ASSERT(container);	m_pContainer	= container;
}

void CWeapon::Load(CInifile* ini, const char* section)
{
	// verify class
	LPCSTR Class		= pSettings->ReadSTRING(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	CObject::Load(ini,section);

	SectorMode		= EPM_AT_LOAD;
	if (pSector)	pSector->objectRemove	(this);

	fTimeToFire		= ini->ReadFLOAT	(section,"rpm");
	fTimeToFire		= 60 / fTimeToFire;

	LPCSTR	name	= ini->ReadSTRING	(section,"wm_name");
	if (name=="")	hWallmark = 0; 
	else			hWallmark = Device.Shader.Create("wallmark",name);
	fWallmarkSize	= ini->ReadFLOAT	(section,"wm_size");

	LPCSTR hud_sect= ini->ReadSTRING	(section,"hud");
	m_pHUD->Load	(ini,hud_sect);
	iAmmoElapsed	= ini->ReadINT		(section,"startup_ammo");
	iAmmoLimit		= ini->ReadINT		(section,"ammo_limit");

	fireDistance	= ini->ReadFLOAT	(section,"fire_distance"	);
	fireDispersion	= ini->ReadFLOAT	(section,"fire_dispersion"	); fireDispersion = deg2rad(fireDispersion);
	iHitPower		= ini->ReadINT		(section,"hit_power"		);
}

void CWeapon::SetDefaults()
{
	iAmmoElapsed	= 0;
	bWorking		= false;
}

void CWeapon::UpdatePosition(const Fmatrix& trans){
	vPosition.set	(trans.c);
	svTransform.set	(trans);
}

void CWeapon::AddShotmark(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	if (0==hWallmark)	return;
	if (R.O)			return;
	::Render.Wallmarks.AddWallmark(
		pCreator->ObjectSpace.GetStaticTris()+R.element,
		vEnd,
		hWallmark,
		fWallmarkSize
	);
}

BOOL CWeapon::FireTrace(Fvector& P, Fvector& D, Collide::ray_query& R)
{
	// direct it by dispersion factor
	Fvector				dir;
	dir.random_dir		(D,fireDispersion,Random);

	// ...and trace line
	m_pParent->bEnabled = false;
	BOOL bResult		= pCreator->ObjectSpace.RayPick( P, dir, fireDistance, R );
	m_pParent->bEnabled = true;
	iAmmoElapsed		--;

	D					= dir;
	return				bResult;
}
