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

	pstrUIIcon	= 0;
	pstrWallmark= 0;
	hUIIcon		= 0;
	hWallmark	= 0;
}

CWeapon::~CWeapon()
{
	_FREE		(m_WpnName);
	_DELETE		(pVisual);
	_DELETE		(m_pHUD);
	
	_FREE		(pstrUIIcon);
	_FREE		(pstrWallmark);
	
	Device.Shader.Delete(hUIIcon);
	if (hWallmark) Device.Shader.Delete(hWallmark);
}

void CWeapon::SetParent	(CEntity* parent, CWeaponList* container)
{
	R_ASSERT(parent);		m_pParent		= parent;
	R_ASSERT(container);	m_pContainer	= container;
}

void CWeapon::Load		(CInifile* ini, const char* section)
{
	// verify class
	LPCSTR Class		= pSettings->ReadSTRING(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	CObject::Load		(ini,section);

	SectorMode			= EPM_AT_LOAD;
	if (pSector)		pSector->objectRemove	(this);

	Fvector				pos,ypr;
	pos					= ini->ReadVECTOR(section,"position");
	ypr					= ini->ReadVECTOR(section,"orientation");
	ypr.mul				(PI/180.f);

	m_Offset.setHPB		(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over(pos);

	fTimeToFire			= ini->ReadFLOAT	(section,"rpm");
	fTimeToFire			= 60 / fTimeToFire;

	LPCSTR	tex			= ini->ReadSTRING	(section,"ui_icon");
	pstrUIIcon			= strdup(tex);
	hUIIcon				= Device.Shader.Create("font",pstrUIIcon);
	
	LPCSTR	name		= ini->ReadSTRING	(section,"wm_name");
	pstrWallmark		= strdup(name);
	if (0==pstrWallmark)hWallmark = 0; 
	else				hWallmark = Device.Shader.Create("effects\\wallmark",pstrWallmark);
	fWallmarkSize		= ini->ReadFLOAT	(section,"wm_size");

	LPCSTR hud_sect		= ini->ReadSTRING	(section,"hud");
	m_pHUD->Load		(ini,hud_sect);
	iAmmoElapsed		= ini->ReadINT		(section,"startup_ammo");
	iAmmoLimit			= ini->ReadINT		(section,"ammo_limit");

	fireDistance		= ini->ReadFLOAT	(section,"fire_distance"	);
	fireDispersion		= ini->ReadFLOAT	(section,"fire_dispersion"	); fireDispersion = deg2rad(fireDispersion);
	fireDispersion_Inc	= ini->ReadFLOAT	(section,"fire_dispersion_add"); 
	fireDispersion_Dec	= ini->ReadFLOAT	(section,"fire_dispersion_relax"); 
	fireDispersion_Current	= 0;

	camRelax			= ini->ReadFLOAT	(section,"cam_relax"		);
	camDispersion		= ini->ReadFLOAT	(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	// tracer
	tracerHeadSpeed		= ini->ReadFLOAT	(section,"tracer_head_speed"	);
	tracerTrailCoeff	= ini->ReadFLOAT	(section,"tracer_trail_scale"	);
	tracerStartLength	= ini->ReadFLOAT	(section,"tracer_start_length"	);
	tracerWidth			= ini->ReadFLOAT	(section,"tracer_width"			);

	// light
	Fvector clr			= ini->ReadVECTOR	(section,"light_color"		);
	light_base.SetColor	(clr.x,clr.y,clr.z);
	light_base.SetRange	(ini->ReadFLOAT	(section,"light_range"		));
	light_var_color		= ini->ReadFLOAT	(section,"light_var_color"	);
	light_var_range		= ini->ReadFLOAT	(section,"light_var_range"	);
	light_lifetime		= 1.f;
	iHitPower			= ini->ReadINT		(section,"hit_power"		);

	bVisible			= FALSE;
}


void CWeapon::OnDeviceCreate()
{
	CObject::OnDeviceCreate();

	if (0==pstrUIIcon)	hUIIcon		= 0;
	else				hUIIcon		= Device.Shader.Create("font",pstrUIIcon);

	if (0==pstrWallmark)hWallmark	= 0; 
	else				hWallmark	= Device.Shader.Create("effects\\wallmark",pstrWallmark);
}
void CWeapon::OnDeviceDestroy()
{
	CObject::OnDeviceDestroy();

	Device.Shader.Delete(hUIIcon);
	Device.Shader.Delete(hWallmark);
}

void CWeapon::Hide		()
{
	FireEnd				();
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
	}else{
		::Render.Wallmarks.AddWallmark(
			pCreator->ObjectSpace.GetStaticTris()+R.element,
			vEnd,
			hWallmark,
			fWallmarkSize
		);
	}
}

void CWeapon::Update		(float dt, BOOL bHUDView)
{
	fireDispersion_Current	-= fireDispersion_Dec*dt;
	clamp					(fireDispersion_Current,0.f,1.f);
	if (light_time>0)		light_time -= dt;
}

BOOL CWeapon::FireTrace		(const Fvector& P, const Fvector& Peff, Fvector& D)
{
	Collide::ray_query	RQ;

	// direct it by dispersion factor
	fireDispersion_Current	+= fireDispersion_Inc;
	clamp				(fireDispersion_Current,0.f,1.f);
	Fvector				dir;
	dir.random_dir		(D,fireDispersion*fireDispersion_Current,Random);

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
				CEntity* E =	dynamic_cast<CEntity*>(RQ.O);
				E->Hit			(iHitPower,D,m_pParent);
			}
		}
		FireShotmark		(D,end_point,RQ);
	}

	// tracer
	Level().Tracers.Add	(Peff,end_point,tracerHeadSpeed,tracerTrailCoeff,tracerStartLength,tracerWidth);

	// light
	Light_Start			();

	return				bResult;
}

void CWeapon::Light_Start	()
{
	if (Device.dwFrame	!= light_frame)
	{
		light_frame					= Device.dwFrame;
		light_time					= light_lifetime;
		
		light_build.SetColor		(Random.randFs(light_var_color,light_base.color.r),Random.randFs(light_var_color,light_base.color.g),Random.randFs(light_var_color,light_base.color.b));
		light_build.SetRange		(Random.randFs(light_var_range,light_base.sphere.R));
	}
}

void CWeapon::Light_Render	(Fvector& P)
{
	float light_scale			= light_time/light_lifetime;
	light_render.SetPosition	(P);
	light_render.SetColor		(light_build.color.r*light_scale,light_build.color.g*light_scale,light_build.color.b*light_scale);
	light_render.SetRange		(light_build.sphere.R*light_scale);
	
	::Render.Lights_Dynamic.Add	(&light_render);
}

void CWeapon::Render(BOOL bHUDView)
{
	if (light_time>0) {
		UpdateFP	(bHUDView);
		Light_Render(vLastFP);
	}
}
