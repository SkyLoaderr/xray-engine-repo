// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "effectorshot.h"
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
	fTimeToFire		= 0;
	iHitPower		= 0;
	STATE			= 0;

	SetDefaults		();
	m_pHUD			= new CWeaponHUD();
	m_WpnName		= strupr(xr_strdup(name));
	m_Offset.identity();

	pstrWallmark	= 0;
	hUIIcon			= 0;
	hWallmark		= 0;

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

	iAmmoLimit		= -1;
	iAmmoCurrent	= -1;
	iAmmoElapsed	= -1;
	iMagazineSize	= -1;

	hud_mode		= FALSE;
}

CWeapon::~CWeapon		()
{
	_FREE				(m_WpnName);
	_DELETE				(pVisual);
	_DELETE				(m_pHUD);
	
	_FREE				(pstrWallmark);
	
	Device.Shader.Delete(hUIIcon);
	if (hWallmark)		Device.Shader.Delete(hWallmark);
}

void CWeapon::animGet	(MotionSVec& lst, LPCSTR prefix)
{
	CMotionDef* M		= m_pHUD->animGet(prefix);
	if (M)				lst.push_back(M);
	for (int i=0; i<MAX_ANIM_COUNT; i++)
	{
		string128		sh_anim;
		sprintf			(sh_anim,"%s%d",prefix,i);
		M				= m_pHUD->animGet(sh_anim);
		if (M)			lst.push_back(M);
	}
}

void CWeapon::SoundCreate(sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq)
{
	string256	name,temp;
	strconcat	(name,"weapons\\",GetName(),"_",s_name);
	if (Engine.FS.Exist(temp,Path.Sounds,name,".wav"))	
	{
		pSounds->Create(dest,TRUE,name,bCtrlFreq,iType);
		return;
	}
	strconcat	(name,"weapons\\","generic_",s_name);
	if (Engine.FS.Exist(temp,Path.Sounds,name,".wav"))	
	{
		pSounds->Create(dest,TRUE,name,bCtrlFreq,iType);
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
	strconcat	(name,"ui\\ui_hud_wpn_",GetName());
	if (Engine.FS.Exist(temp,Path.Textures,name,".dds"))	
	{
		dest = Device.Shader.Create(S,name);
		return;
	}
	strcpy		(name,"ui\\ui_hud_wpn_generic");
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

void CWeapon::UpdateXForm	()
{
	if (Device.dwFrame!=dwXF_Frame){
		dwXF_Frame = Device.dwFrame;

		if (0==H_Parent())	return;

		if (hud_mode)
		{
			if (m_pHUD)
			{
				Fmatrix							trans;
				Level().Cameras.affected_Matrix	(trans);
				m_pHUD->UpdatePosition			(trans);
			}
		} else {
			// Get access to entity and its visual
			CEntityAlive*	E		= dynamic_cast<CEntityAlive*>(H_Parent());
			R_ASSERT		(E);
			CKinematics*	V		= PKinematics	(E->Visual());
			VERIFY			(V);

			// Get matrices
			int				boneL,boneR;
			E->g_WeaponBones(boneL,boneR);
			V->Calculate	();
			Fmatrix& mL		= V->LL_GetTransform(boneL);
			Fmatrix& mR		= V->LL_GetTransform(boneR);

			// Calculate
			Fmatrix			mRes;
			Fvector			R,D,N;
			D.sub			(mL.c,mR.c);	D.normalize_safe();
			R.crossproduct	(mR.j,D);		R.normalize_safe();
			N.crossproduct	(D,R);			N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mulA_43	(E->clXFORM());
			UpdatePosition	(mRes);
		}
	}
}

void CWeapon::UpdateFP		()
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm		();

		if (hud_mode && (0!=H_Parent()))	
		{
			// 1st person view - skeletoned
			CKinematics* V			= PKinematics(m_pHUD->Visual());
			V->Calculate			();

			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(m_pHUD->iFireBone);
			Fmatrix& parent			= m_pHUD->Transform	();
			Fvector& fp				= m_pHUD->vFirePoint;
			Fvector& sp				= m_pHUD->vShellPoint;
			fire_mat.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastFP);
			fire_mat.transform_tiny	(vLastSP,sp);
			parent.transform_tiny	(vLastSP);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		} else {
			// 3rd person or no parent
			Fmatrix& parent			= svTransform;
			Fvector& fp				= vFirePoint;
			Fvector& sp				= vShellPoint;
			parent.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastSP,sp);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		}
	}
}

void CWeapon::Load		(LPCSTR section)
{
	// verify class
	LPCSTR Class		= pSettings->ReadSTRING(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT			(load_cls==SUB_CLS_ID);

	inherited::Load		(section);

	Fvector				pos,ypr;
	pos					= pSettings->ReadVECTOR(section,"position");
	ypr					= pSettings->ReadVECTOR(section,"orientation");
	ypr.mul				(PI/180.f);

	m_Offset.setHPB		(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over(pos);

	fTimeToFire			= pSettings->ReadFLOAT	(section,"rpm");
	fTimeToFire			= 60 / fTimeToFire;

	ShaderCreate		(hUIIcon,"hud\\default","");
	
	LPCSTR	name		= pSettings->ReadSTRING	(section,"wm_name");
	pstrWallmark		= xr_strdup(name);
	if (0==pstrWallmark)hWallmark = 0; 
	else				hWallmark = Device.Shader.Create("effects\\wallmark",pstrWallmark);
	fWallmarkSize		= pSettings->ReadFLOAT		(section,"wm_size");

	LPCSTR hud_sect		= pSettings->ReadSTRING		(section,"hud");
	m_pHUD->Load		(hud_sect);

	iAmmoLimit			= pSettings->ReadINT		(section,"ammo_limit"		);
	iAmmoCurrent		= pSettings->ReadINT		(section,"ammo_current"		);
	iAmmoElapsed		= pSettings->ReadINT		(section,"ammo_elapsed"		);
	iMagazineSize		= pSettings->ReadINT		(section,"ammo_mag_size"	);
	
	fireDistance		= pSettings->ReadFLOAT		(section,"fire_distance"	);
	fireDispersionBase	= pSettings->ReadFLOAT		(section,"fire_dispersion_base"	);	fireDispersionBase	= deg2rad(fireDispersionBase);
	fireDispersion		= pSettings->ReadFLOAT		(section,"fire_dispersion"	);		fireDispersion		= deg2rad(fireDispersion);
	fireDispersion_Inc	= pSettings->ReadFLOAT		(section,"fire_dispersion_add"); 
	fireDispersion_Dec	= pSettings->ReadFLOAT		(section,"fire_dispersion_relax"); 
	fireDispersion_Current	= 0;

	camMaxAngle			= pSettings->ReadFLOAT		(section,"cam_max_angle"	); camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed		= pSettings->ReadFLOAT		(section,"cam_relax_speed"	); camRelaxSpeed = deg2rad(camRelaxSpeed);
	camDispersion		= pSettings->ReadFLOAT		(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);

	dispVelFactor		= pSettings->ReadFLOAT		(section,"disp_vel_factor"	);
	dispJumpFactor		= pSettings->ReadFLOAT		(section,"disp_jump_factor"	);
	dispCrouchFactor	= pSettings->ReadFLOAT		(section,"disp_crouch_factor");

	// tracer
	tracerHeadSpeed		= pSettings->ReadFLOAT		(section,"tracer_head_speed"	);
	tracerTrailCoeff	= pSettings->ReadFLOAT		(section,"tracer_trail_scale"	);
	tracerStartLength	= pSettings->ReadFLOAT		(section,"tracer_start_length"	);
	tracerWidth			= pSettings->ReadFLOAT		(section,"tracer_width"			);

	// light
	Fvector clr			= pSettings->ReadVECTOR		(section,"light_color"		);
	light_base.SetColor	(clr.x,clr.y,clr.z);
	light_base.SetRange	(pSettings->ReadFLOAT		(section,"light_range"		));
	light_var_color		= pSettings->ReadFLOAT		(section,"light_var_color"	);
	light_var_range		= pSettings->ReadFLOAT		(section,"light_var_range"	);
	light_lifetime		= pSettings->ReadFLOAT		(section,"light_time"		);
	light_time			= -1.f;
	iHitPower			= pSettings->ReadINT		(section,"hit_power"		);

	vFirePoint			= pSettings->ReadVECTOR		(section,"fire_point"		);
	vShellPoint			= pSettings->ReadVECTOR		(section,"shell_point"		);

	// flames
	iFlameDiv			= pSettings->ReadINT		(section,"flame_div"		);
	fFlameLength		= pSettings->ReadFLOAT		(section,"flame_length"		);
	fFlameSize			= pSettings->ReadFLOAT		(section,"flame_size"		);

	setVisible			(FALSE);
}

BOOL CWeapon::net_Spawn		(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	BOOL bResult			= inherited::net_Spawn	(bLocal,server_id,o_pos,o_angle,P,flags);

	u16						current,elapsed;
	P.r_u16					(current);	iAmmoCurrent	= current;
	P.r_u16					(elapsed);	iAmmoElapsed	= elapsed;

	setVisible				(true);
	setEnabled				(true);

	return bResult;
}

void CWeapon::net_Destroy	()
{
	inherited::net_Destroy	();
}

void CWeapon::net_Export	(NET_Packet& P)
{
	u8 flags				=	u8(IsUpdating()? M_UPDATE_WEAPON_wfVisible:0);
	flags					|=	u8(IsWorking() ? M_UPDATE_WEAPON_wfWorking:0);

	P.w_u32					(Level().timeServer());
	P.w_u8					(flags);
	P.w_u8					(u8(STATE));

	P.w_u16					(u16(iAmmoCurrent));
	P.w_u16					(u16(iAmmoElapsed));

	P.w_vec3				(svTransform.c);

	float					_x,_y,_z;
	svTransform.getHPB		(_y,_x,_z);
	P.w_angle8				(_x);
	P.w_angle8				(_y);
	P.w_angle8				(_z);

	UpdateFP				();
	P.w_vec3				(vLastFP);
	P.w_dir					(vLastFD);
}

void CWeapon::net_Import	(NET_Packet& P)
{
	net_update				N;

	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(N.flags);
	P.r_u8					(N.state);

	P.r_u16					(N.ammo_current);
	P.r_u16					(N.ammo_elapsed);

	P.r_vec3				(N.pos);
	P.r_angle8				(N.angles.x);
	P.r_angle8				(N.angles.y);
	P.r_angle8				(N.angles.z);

	P.r_vec3				(N.fpos);
	P.r_dir					(N.fdir);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	
	{
		NET.push_back			(N);
	}
}

void CWeapon::Update		(DWORD dT)
{
	// Queue shrink
	DWORD	dwTimeCL		= Level().timeServer()-NET_Latency;
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	// Logic
	float dt				= float(dT)/1000.f;
	fireDispersion_Current	-=	fireDispersion_Dec*dt;
	clamp					(fireDispersion_Current,0.f,1.f);
	if (light_time>0)		light_time -= dt;

	// Inherited
	inherited::Update		(dT);
}

void CWeapon::OnH_B_Independent	()
{
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	hud_mode					= FALSE;
	UpdateXForm					();
}

void CWeapon::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	setVisible					(false);
	setEnabled					(false);
}

void CWeapon::net_update::lerp(CWeapon::net_update& A, CWeapon::net_update& B, float f)
{
	float invf		= 1.f-f;
	flags			= (f<0.5f)?A.flags:B.flags;
	state			= (f<0.5f)?A.state:B.state;
	ammo_current	= u16(iFloor(invf*float(A.ammo_current)+f*float(B.ammo_current)));
	ammo_elapsed	= u16(iFloor(invf*float(A.ammo_elapsed)+f*float(B.ammo_elapsed)));
	pos.lerp		(A.pos,B.pos,f);
	angles.x		= CEntity::u_lerp_angle	(A.angles.x,B.angles.x,	f);
	angles.y		= CEntity::u_lerp_angle	(A.angles.y,B.angles.y,	f);
	angles.z		= CEntity::u_lerp_angle	(A.angles.z,B.angles.z,	f);
	fpos.lerp		(A.fpos,B.fpos,f);
	fdir.lerp		(A.fdir,B.fdir,f);	fdir.normalize	();
}

void CWeapon::UpdateCL		()
{
	inherited::UpdateCL		();

	if (Remote() && NET.size())
	{
		// distinguish interpolation/extrapolation
		DWORD	dwTime		= Level().timeServer()-NET_Latency;
		net_update&	N		= NET.back();
		if ((dwTime > N.dwTimeStamp) || (NET.size()<2))
		{
			// BAD.	extrapolation
			NET_Last		= N;
		} else {
			// OK.	interpolation

			// Search 2 keyframes for interpolation
			int select		= -1;
			for (DWORD id=0; id<NET.size()-1; id++)
			{
				if ((NET[id].dwTimeStamp<=dwTime)&&(dwTime<=NET[id+1].dwTimeStamp))	select=id;
			}
			if (select>=0)		
			{
				// Interpolate state
				net_update&	A		= NET[select+0];
				net_update&	B		= NET[select+1];
				DWORD	d1			= dwTime-A.dwTimeStamp;
				DWORD	d2			= B.dwTimeStamp - A.dwTimeStamp;
				float	factor		= (float(d1)/float(d2));
				NET_Last.lerp		(A,B,factor);

				// 
				iAmmoCurrent		= NET_Last.ammo_current;
				iAmmoElapsed		= NET_Last.ammo_elapsed;
				if (NET_Last.flags&M_UPDATE_WEAPON_wfWorking)
				{
					if (!IsWorking())	{ FireStart(); }
				} else {
					if (IsWorking())	{ FireEnd(); }
				}
				if (DWORD(NET_Last.state)!=STATE)	OnStateSwitch(NET_Last.state);
			}
		}
	}
}

void CWeapon::SwitchState(DWORD S)
{
	if (Local() && (S!=STATE))	
	{
		OnStateSwitch	(S);
	}
}

void CWeapon::OnVisible		()
{
	if (light_time>0) 
	{
		UpdateFP	();
		Light_Render(vLastFP);
	}
	if (m_pHUD && hud_mode)	PKinematics(m_pHUD->Visual())->Update	();
}

void CWeapon::OnDeviceCreate	()
{
	inherited::OnDeviceCreate();

	ShaderCreate				(hUIIcon,"hud\\default","");

	if (0==pstrWallmark)		hWallmark	= 0; 
	else						hWallmark	= Device.Shader.Create("effects\\wallmark",pstrWallmark);
}

void CWeapon::OnDeviceDestroy	()
{
	inherited::OnDeviceDestroy	();
	
	ShaderDestroy				(hUIIcon);
	Device.Shader.Delete		(hWallmark);
}

void CWeapon::Hide				()
{
	FireEnd							();
	OnHide							();
	bPending						= TRUE;

	// add shot effector
	Level().Cameras.RemoveEffector	(cefShot);
}

void CWeapon::signal_HideComplete()
{
	setVisible		(FALSE);
}

void CWeapon::Show		()
{
	OnShow			();
	setVisible		(TRUE);
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

BOOL CWeapon::FireTrace		(const Fvector& P, const Fvector& Peff, Fvector& D)
{
	Collide::ray_query		RQ;

	// direct it by dispersion factor
	Fvector					dir;
	dir.random_dir			(D,(fireDispersionBase+fireDispersion*fireDispersion_Current)*GetPrecision(),Random);

	// increase dispersion
	fireDispersion_Current	+= fireDispersion_Inc;
	clamp					(fireDispersion_Current,0.f,1.f);

	// ...and trace line
	H_Parent()->setEnabled	(false);
	BOOL bResult			= Level().ObjectSpace.RayPick( P, dir, fireDistance, RQ );
	H_Parent()->setEnabled	(true);
	D						= dir;

	// ...analyze
	Fvector end_point; 
	end_point.mad		(P,D,RQ.range);
	if (bResult)
	{
		if (Local() && RQ.O) 
		{
			// 
			NET_Packet		P;
			u_EventGen		(P,GE_HIT,RQ.O->ID());
			P.w_u16			(H_Parent()->ID());
			P.w_dir			(D);
			P.w_float		(iHitPower);
			u_EventSend		(P);
		}
		FireShotmark		(D,end_point,RQ);
	}

	// tracer
	if (tracerFrame != Device.dwFrame) 
	{
		tracerFrame = Device.dwFrame;
		Level().Tracers.Add	(Peff,end_point,tracerHeadSpeed,tracerTrailCoeff,tracerStartLength,tracerWidth);
	}

	// light
	Light_Start			();
	
	// Ammo
	if (Local()) 
	{
		iAmmoElapsed	--;
		if (iAmmoElapsed==0) 
		{
			OnMagazineEmpty	();
		}
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

void CWeapon::OnDrawFlame	()
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
			::Render->add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,hud_mode);
			P.add(D);
		}
		fFlameTime -= Device.fTimeDelta;
	}
}

