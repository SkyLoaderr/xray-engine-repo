// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "effectorshot.h"
#include "..\bodyinstance.h"
#include "..\render.h"
#include "..\portal.h"
#include "..\PSObject.h"
#include "Weapon.h"
#include "WeaponHUD.h"

#include "entity.h"


#define FLAME_TIME 0.05f
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeapon::CWeapon(LPCSTR name)
{
	fTimeToFire	= 0;
	iHitPower	= 0;
	bVisible	= false;
	SetDefaults	();
	m_pHUD		= new CWeaponHUD();
	m_WpnName	= strupr(strdup(name));
	m_pContainer= 0;
	m_Offset.identity();

	pstrWallmark= 0;
	hUIIcon		= 0;
	hWallmark	= 0;

	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);
	vLastSP.set		(0,0,0);

	iFlameDiv		= 0;
	fFlameLength	= 0;
	fFlameSize		= 0;
	fFlameTime		= -1;

	dispVelFactor	= 0.2f;
	dispJumpFactor	= 4.f;
	dispCrouchFactor= 0.75f;
}

CWeapon::~CWeapon()
{
	_FREE		(m_WpnName);
	_DELETE		(pVisual);
	_DELETE		(m_pHUD);
	
	_FREE		(pstrWallmark);
	
	Device.Shader.Delete(hUIIcon);
	if (hWallmark) Device.Shader.Delete(hWallmark);
}

void CWeapon::SoundCreate(sound& dest, LPCSTR s_name, int iType)
{
	string256	name,temp;
	strconcat	(name,"weapons\\",GetName(),"_",s_name);
	if (Engine.FS.Exist(temp,Path.Sounds,name,".wav"))	
	{
		pSounds->Create3D(dest,name,FALSE,iType);
		return;
	}
	strconcat	(name,"weapons\\","generic_",s_name);
	if (Engine.FS.Exist(temp,Path.Sounds,name,".wav"))	
	{
		pSounds->Create3D(dest,name,FALSE,iType);
		return;
	}
	Device.Fatal("Can't find sound '%s' for weapon '%s'",name,GetName());
}
void CWeapon::SoundDestroy	(	sound& dest)
{
	pSounds->Delete			(dest);
}
void CWeapon::ShaderCreate	(Shader* &dest, LPCSTR S, LPCSTR T)
{
	if (dest)	return;
	string256	name,temp;

	// test 'WEAPONS' folder
	strconcat	(name,"weapons\\",GetName(),"\\",T);
	if (Engine.FS.Exist(temp,Path.Textures,name,".dds"))	
	{
		dest = Device.Shader.Create(S,name);
		return;
	}
	strconcat	(name,"weapons\\generic\\",T);
	if (Engine.FS.Exist(temp,Path.Textures,name,".dds"))	
	{
		dest = Device.Shader.Create(S,name);
		return;
	}

	// test 'UI' folder
	strconcat	(name,"ui\\hud_wpn_",GetName());
	if (Engine.FS.Exist(temp,Path.Textures,name,".dds"))	
	{
		dest = Device.Shader.Create(S,name);
		return;
	}
	strcpy		(name,"ui\\hud_wpn_generic");
	if (Engine.FS.Exist(temp,Path.Textures,name,".dds"))	
	{
		dest = Device.Shader.Create(S,name);
		return;
	}
	Device.Fatal("Can't find texture '%s' for weapon '%s'",T,GetName());
}
void CWeapon::ShaderDestroy	(Shader* &dest)
{
	if (0==dest)	return;
	Device.Shader.Delete	(dest);
}

float CWeapon::GetPrecision()
{
	CEntity* E	=	dynamic_cast<CEntity*>(H_Parent());
	VERIFY			(E);
	float prec	=	E->g_Accuracy();
	CEntity::SEntityState state;
	if (E->g_State(state))
	{
		prec *= (1.f+state.fVelocity*dispVelFactor);
		if (state.bJump)		prec*=dispJumpFactor;
		else if (state.bCrouch)	prec*=dispCrouchFactor;
	}

	return prec;
}

void CWeapon::SetParent	(CEntity* parent, CWeaponList* container)
{
	R_ASSERT(parent);		Parent			= parent;
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

	ShaderCreate		(hUIIcon,"hud\\default","");
	
	LPCSTR	name		= ini->ReadSTRING	(section,"wm_name");
	pstrWallmark		= strdup(name);
	if (0==pstrWallmark)hWallmark = 0; 
	else				hWallmark = Device.Shader.Create("effects\\wallmark",pstrWallmark);
	fWallmarkSize		= ini->ReadFLOAT	(section,"wm_size");

	LPCSTR hud_sect		= ini->ReadSTRING	(section,"hud");
	m_pHUD->Load		(ini,hud_sect);

	iAmmoLimit			= ini->ReadINT		(section,"ammo_limit"		);
	iAmmoCurrent		= ini->ReadINT		(section,"ammo_current"		);
	iAmmoElapsed		= ini->ReadINT		(section,"ammo_elapsed"		);
	iMagazineSize		= ini->ReadINT		(section,"ammo_mag_size"	);
	
	fireDistance		= ini->ReadFLOAT	(section,"fire_distance"	);
	fireDispersionBase	= ini->ReadFLOAT	(section,"fire_dispersion_base"	);	fireDispersionBase	= deg2rad(fireDispersionBase);
	fireDispersion		= ini->ReadFLOAT	(section,"fire_dispersion"	);		fireDispersion		= deg2rad(fireDispersion);
	fireDispersion_Inc	= ini->ReadFLOAT	(section,"fire_dispersion_add"); 
	fireDispersion_Dec	= ini->ReadFLOAT	(section,"fire_dispersion_relax"); 
	fireDispersion_Current	= 0;

	camMaxAngle			= ini->ReadFLOAT	(section,"cam_max_angle"	); camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed		= ini->ReadFLOAT	(section,"cam_relax_speed"	); camRelaxSpeed = deg2rad(camRelaxSpeed);
	camDispersion		= ini->ReadFLOAT	(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	dispVelFactor		= ini->ReadFLOAT	(section,"disp_vel_factor"	);
	dispJumpFactor		= ini->ReadFLOAT	(section,"disp_jump_factor"	);
	dispCrouchFactor	= ini->ReadFLOAT	(section,"disp_crouch_factor");

	// tracer
	tracerHeadSpeed		= ini->ReadFLOAT	(section,"tracer_head_speed"	);
	tracerTrailCoeff	= ini->ReadFLOAT	(section,"tracer_trail_scale"	);
	tracerStartLength	= ini->ReadFLOAT	(section,"tracer_start_length"	);
	tracerWidth			= ini->ReadFLOAT	(section,"tracer_width"			);

	// light
	Fvector clr			= ini->ReadVECTOR	(section,"light_color"		);
	light_base.SetColor	(clr.x,clr.y,clr.z);
	light_base.SetRange	(ini->ReadFLOAT		(section,"light_range"		));
	light_var_color		= ini->ReadFLOAT	(section,"light_var_color"	);
	light_var_range		= ini->ReadFLOAT	(section,"light_var_range"	);
	light_lifetime		= ini->ReadFLOAT	(section,"light_time"		);
	light_time			= -1.f;
	iHitPower			= ini->ReadINT		(section,"hit_power"		);

	vFirePoint			= ini->ReadVECTOR	(section,"fire_point"		);
	vShellPoint			= ini->ReadVECTOR	(section,"shell_point"		);

	// flames
	iFlameDiv			= ini->ReadINT		(section,"flame_div"		);
	fFlameLength		= ini->ReadFLOAT	(section,"flame_length"		);
	fFlameSize			= ini->ReadFLOAT	(section,"flame_size"		);

	bVisible			= FALSE;
}

void CWeapon::OnDeviceCreate()
{
	CObject::OnDeviceCreate();

	ShaderCreate		(hUIIcon,"hud\\default","");

	if (0==pstrWallmark)hWallmark	= 0; 
	else				hWallmark	= Device.Shader.Create("effects\\wallmark",pstrWallmark);
}
void CWeapon::OnDeviceDestroy()
{
	CObject::OnDeviceDestroy();
	
	ShaderDestroy		(hUIIcon);
	Device.Shader.Delete(hWallmark);
}

void CWeapon::Hide		()
{
	FireEnd				();
	OnHide				();
	bPending			= TRUE;
	// add shot effector
	Level().Cameras.RemoveEffector	(cefShot);
}
void CWeapon::signal_HideComplete()
{
	bVisible		= FALSE;
}
void CWeapon::Show		()
{
	OnShow				();
	bVisible		= TRUE;
}

void CWeapon::SetDefaults()
{
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
	
	if (R.O) {
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
			CSector* S = R.O->Sector();
			Fvector D;	D.invert(vDir);

			LPCSTR ps_gibs		= "blood_1";//(Random.randI(5)==0)?"sparks_1":"stones";
			CPSObject* PS		= new CPSObject(ps_gibs,S,true);
			PS->m_Emitter.m_ConeDirection.set(D);
			PS->PlayAtPos		(vEnd);
		}
	} else {
		::Render->add_Wallmark	(
			hWallmark,
			vEnd,
			fWallmarkSize,
			pCreator->ObjectSpace.GetStaticTris()+R.element);
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
	Fvector				dir;
	dir.random_dir		(D,(fireDispersionBase+fireDispersion*fireDispersion_Current)*GetPrecision(),Random);
	// increase dispersion
	fireDispersion_Current	+= fireDispersion_Inc;
	clamp				(fireDispersion_Current,0.f,1.f);

	// ...and trace line
	H_Parent()->setEnabled(false);
	BOOL bResult		= pCreator->ObjectSpace.RayPick( P, dir, fireDistance, RQ );
	H_Parent()->setEnabled(true);
	D					= dir;

	// ...analyze
	Fvector end_point; 
	end_point.mad		(P,D,RQ.range);
	if (bResult)
	{
		if (RQ.O) {
			if (H_Parent()->Local() && (RQ.O->CLS_ID==CLSID_ENTITY))
			{
				CEntity* E =	dynamic_cast<CEntity*>(RQ.O);
				E->Hit			(iHitPower,D,dynamic_cast<CEntity*>(H_Parent()));
			}
		}
		FireShotmark		(D,end_point,RQ);
	}

	// tracer
	Level().Tracers.Add	(Peff,end_point,tracerHeadSpeed,tracerTrailCoeff,tracerStartLength,tracerWidth);

	// light
	Light_Start			();
	
	// Ammo
	iAmmoElapsed	--;
	if (iAmmoElapsed==0) 
	{
		OnMagazineEmpty	();
	}
	
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
	
	::Render->L_add				(&light_render);
}

void CWeapon::Render(BOOL bHUDView)
{
	if (light_time>0) {
		UpdateFP	(bHUDView);
		Light_Render(vLastFP);
	}
	if (m_pHUD)	PKinematics(m_pHUD->Visual())->Update	();
}

void CWeapon::OnDrawFlame(BOOL bHUDView)
{
	if (fFlameTime>0)	
	{
		// fire flash
		Fvector P = vLastFP;
		float k=fFlameTime/FLAME_TIME;
		Fvector D; D.mul(vLastFD,::Random.randF(fFlameLength*k)/float(iFlameDiv));
		float f = fFlameSize;
		for (int i=0; i<iFlameDiv; i++)
		{
			f		*= 0.9f;
			float	S = f+f*::Random.randF	();
			float	A = ::Random.randF		(PI_MUL_2);
			::Render->add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,bHUDView);
			P.add(D);
		}
		fFlameTime -= Device.fTimeDelta;
	}
}

