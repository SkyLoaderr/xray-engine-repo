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
	Device.Shader.Delete(hUIIcon);
	if (hWallmark) Device.Shader.Delete(hWallmark);
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

	CObject::Load	(ini,section);

	SectorMode		= EPM_AT_LOAD;
	if (pSector)	pSector->objectRemove	(this);

	fTimeToFire		= ini->ReadFLOAT	(section,"rpm");
	fTimeToFire		= 60 / fTimeToFire;

	LPCSTR	tex		= ini->ReadSTRING	(section,"ui_icon");
	hUIIcon			= Device.Shader.Create("font",tex);
	
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
	
	bVisible		= FALSE;
}

void CWeapon::Hide		()
{
	FireEnd			();
}
void CWeapon::signal_HideComplete()
{
	bVisible		= FALSE;
}
void CWeapon::Show		()
{
	bVisible		= TRUE;
}

void CWeapon::SetDefaults()
{
	iAmmoElapsed	= 0;
	bWorking		= false;
	bPending		= false;
}

void CWeapon::UpdatePosition(const Fmatrix& trans){
	vPosition.set	(trans.c);
	svTransform.set	(trans);
}

void CWeapon::FireShotmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) 
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

BOOL CWeapon::FireTrace		(const Fvector& P, const Fvector& Peff, Fvector& D)
{
	Collide::ray_query	RQ;

	// direct it by dispersion factor
	Fvector				dir;
	dir.random_dir		(D,fireDispersion,Random);

	// ...and trace line
	m_pParent->bEnabled = false;
	BOOL bResult		= pCreator->ObjectSpace.RayPick( P, dir, fireDistance, RQ );
	m_pParent->bEnabled = true;
	D					= dir;

	// ...analyze
	Fvector end_point; 
	end_point.direct	(P,D,RQ.range);
	if (bResult)
	{
		if (RQ.O) {
			if (m_pParent->Local() && (RQ.O->CLS_ID==CLSID_ENTITY))
			{
				CEntity* E =	(CEntity*)RQ.O;
				E->Hit			(iHitPower,D,m_pParent);
			}
		} else {
			FireShotmark		(D,end_point,RQ);
		}
	}

	// tracer
	Fcolor		c; c.set(1,.7f,.1f,1);
	Level().Tracers.Add	(Peff,end_point,500,0.1f,2,c);

	return				bResult;
}
