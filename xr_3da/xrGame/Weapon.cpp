// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\bodyinstance.h"
#include "..\fstaticrender.h"
#include "..\PSObject.h"
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
	m_Offset.identity();
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

	Fvector			pos,ypr;
	pos				= ini->ReadVECTOR(section,"position");
	ypr				= ini->ReadVECTOR(section,"orientation");
	ypr.mul			(PI/180.f);

	m_Offset.setHPB	(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over(pos);


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

	camRelax		= ini->ReadFLOAT	(section,"cam_relax"		);
	camDispersion	= ini->ReadFLOAT	(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	// tracer
	tracerHeadSpeed	= ini->ReadFLOAT	(section,"tracer_head_speed"	);
	tracerTrailCoeff= ini->ReadFLOAT	(section,"tracer_trail_scale"	);
	tracerStartLength= ini->ReadFLOAT	(section,"tracer_start_length"	);
	tracerWidth		= ini->ReadFLOAT	(section,"tracer_width"			);

	// light
	Fvector clr		= ini->ReadVECTOR	(section,"light_color"		);
	light_base.SetColor	(clr.x,clr.y,clr.z);
	light_base.SetRange	(ini->ReadFLOAT	(section,"light_range"		));
	light_var_color	= ini->ReadFLOAT	(section,"light_var_color"	);
	light_var_range	= ini->ReadFLOAT	(section,"light_var_range"	);

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
	svTransform.mul	(trans,m_Offset);
}

void CWeapon::FireShotmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	if (0==hWallmark)	return;
	if (R.O && (R.O->CLS_ID==CLSID_ENTITY)){
		// particles
		Fvector D;
		D.invert(vDir);

		CSector* S			= R.O->Sector();

		// stones or sparks
		LPCSTR ps_gibs		= "sparks_1";//(Random.randI(5)==0)?"sparks_1":"stones";
		CPSObject* PS		= new CPSObject(ps_gibs,S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->PlayAtPos		(vEnd);
/*
		// smoke
		PS					= new CPSObject("smokepuffs_1",S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->PlayAtPos		(vEnd);
*/
	}else{
		::Render.Wallmarks.AddWallmark(
			pCreator->ObjectSpace.GetStaticTris()+R.element,
			vEnd,
			hWallmark,
			fWallmarkSize
		);
	}
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
		}
		FireShotmark		(D,end_point,RQ);
	}

	// tracer
	Level().Tracers.Add	(Peff,end_point,tracerHeadSpeed,tracerTrailCoeff,tracerStartLength,tracerWidth);

	// light
	if (Device.dwFrame	!= light_frame)
	{
		light_frame		= Device.dwFrame;
		
		light_render.SetPosition	(Peff);
		light_render.SetColor		(Random.randFs(light_var_color,light_base.color.r),Random.randFs(light_var_color,light_base.color.g),Random.randFs(light_var_color,light_base.color.b));
		light_render.SetRange		(Random.randFs(light_var_range,light_base.sphere.R));

		::Render.Lights_Dynamic.Add	(&light_render);
	}

	return				bResult;
}
