// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "effectorshot.h"
#include "Weapon.h"
#include "WeaponHUD.h"

//#include "PSObject.h"
#include "ParticlesObject.h"

#include "HUDManager.h"

#include "entity.h"
#include "actor.h"


#define FLAME_TIME 0.05f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
//���������� ������������� � ����������
//m - ���.��������, s - ������������������ ����������
static float RandNormal(float m, float s)
{
	const float epsilon = 0.00000000001f;
	float r1, r2;

	r1 = ::Random.randF(0.f,1.0f);
	r2 = ::Random.randF(0.f,1.0f);
	
	float norm_num = (-2 * log(r1<epsilon?epsilon:r1) * _cos (2 * PI * r2));

	norm_num = m + norm_num*s;
};
*/

CWeapon::CWeapon(LPCSTR name)
{
	fTimeToFire			= 0;
	iHitPower			= 0;
	STATE				= NEXT_STATE		= eHidden;

	SetDefaults			();
	m_pHUD				= xr_new<CWeaponHUD> ();
	m_WpnName			= name;
	m_Offset.identity	();

	pstrWallmark		= 0;
	hUIIcon				= 0;
	hWallmark			= 0;

	vLastFP.set			(0,0,0);
	vLastFD.set			(0,0,0);
	vLastSP.set			(0,0,0);

	iFlameDiv			= 0;
	fFlameLength		= 0;
	fFlameSize			= 0;
	fFlameTime			= -1;

	dispVelFactor		= 0.2f;
	dispJumpFactor		= 4.f;
	dispCrouchFactor	= 0.75f;

	iAmmoElapsed		= -1;
	iMagazineSize		= -1;
	iBuckShot			= 1;
	m_ammoType			= 0;
	m_ammoName			= NULL;

	m_pPhysicsShell		= 0;
	hud_mode			= FALSE;
	
	eHandDependence		= hdNone;

	fZoomFactor			= DEFAULT_FOV;

	m_pAmmo				= NULL;

	light_render				= ::Render->light_create();
	light_render->set_shadow	(true);
	m_shotLight = true;

	m_bUsingCondition = true;
	bMisfire = false;

	m_flagsAddOnState = 0;
}

CWeapon::~CWeapon		()
{
	::Render->light_destroy	(light_render);

	xr_delete			(m_pHUD);
	xr_delete			(m_pPhysicsShell);

	hUIIcon.destroy		();
	hWallmark.destroy	();
}

void CWeapon::Hit(float P, Fvector &dir,	
		    CObject* who, s16 element,
		    Fvector position_in_object_space, 
		    float impulse, 
		    ALife::EHitType hit_type)
{
	inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);
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
	R_ASSERT2(!lst.empty(),prefix);
}

void CWeapon::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq)
{
	string256	name,temp;
	strconcat	(name,"weapons\\",GetName(),"_",s_name,".wav");
	if (FS.exist(temp,"$game_sounds$",name)) 
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	strconcat	(name,"weapons\\","generic_",s_name,".wav");
	if (FS.exist(temp,"$game_sounds$",name))	
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	Debug.fatal	("Can't find ref_sound '%s' for weapon '%s'",name,GetName());
}

void CWeapon::SoundDestroy	(	ref_sound& dest)
{
	dest.destroy			();
}

void CWeapon::ShaderCreate	(ref_shader &dest, LPCSTR S, LPCSTR T)
{
	if	(dest)	return;
	string256	name,temp;

	// test 'WEAPONS' folder 
	strconcat	(name,"weapons\\",GetName(),"\\",T);
	if (FS.exist(temp,"$game_textures$",name,".dds"))	
	{
		dest.create(S,name); 
		return;
	}
	strconcat	(name,"weapons\\generic\\",T);
	if (FS.exist(temp,"$game_textures$",name,".dds"))	
	{
		dest.create(S,name); 
		return;
	}

	// test 'UI' folder
	strconcat	(name,"ui\\ui_hud_wpn_",GetName());
	if (FS.exist(temp,"$game_textures$",name,".dds"))	
	{
		dest.create(S,name); 
		return;
	}
	strcpy		(name,"ui\\ui_hud_wpn_generic");
	if (FS.exist(temp,"$game_textures$",name,".dds"))	
	{
		dest.create(S,name); 
		return;
	}
	Debug.fatal	("Can't find texture '%s' for weapon '%s'",T,GetName());
}

void CWeapon::ShaderDestroy	(ref_shader &dest)
{
	dest.destroy			();
}

float CWeapon::GetPrecision	()
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

		// Get access to entity and its visual
		CEntityAlive*	E		= dynamic_cast<CEntityAlive*>(H_Parent());
		
		if(!E) return;
		R_ASSERT		(E);
		CKinematics*	V		= PKinematics	(E->Visual());
		VERIFY			(V);

		// Get matrices
		int				boneL,boneR,boneR2;
		E->g_WeaponBones(boneL,boneR,boneR2);
		if ((STATE == eReload) || (!E->g_Alive()))
			boneL = boneR2;
		V->Calculate	();
		Fmatrix& mL		= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR		= V->LL_GetTransform(u16(boneR));
		// Calculate
		Fmatrix			mRes;
		Fvector			R,D,N;
		D.sub			(mL.c,mR.c);	D.normalize_safe();
		R.crossproduct	(mR.j,D);		R.normalize_safe();
		N.crossproduct	(D,R);			N.normalize_safe();
		mRes.set		(R,N,D,mR.c);
		mRes.mulA_43	(E->XFORM());
		UpdatePosition	(mRes);

		//
		if (hud_mode)
		{
			if (m_pHUD)
			{
				Fmatrix							trans;
				Level().Cameras.affected_Matrix	(trans);
				m_pHUD->UpdatePosition			(trans);
			}
		} else {
		}
	}
}

void CWeapon::UpdateFP		()
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm		();

		if (hud_mode && (0!=H_Parent()) && Local())
		{
			// 1st person view - skeletoned
			CKinematics* V			= PKinematics(m_pHUD->Visual());
			V->Calculate			();

			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(u16(m_pHUD->iFireBone));
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
			Fmatrix& parent			= XFORM();
			Fvector& fp				= vFirePoint;
			Fvector& sp				= vShellPoint;
			parent.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastSP,sp);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
/*			HUD().pHUDFont->Color	(0xffffffff);
			HUD().pHUDFont->OutSet	(400,300);
			HUD().pHUDFont->OutNext	("%3.2f,  %3.2f,  %3.2f",vLastFD.x,vLastFD.y,vLastFD.z);
*/		}
	}
}

void CWeapon::Load		(LPCSTR section)
{
	// verify class
	LPCSTR Class		= pSettings->r_string		(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT			(load_cls==SUB_CLS_ID);

	//����������� ��������� ������������� ����� ��������� ��� ���� ����� ������
	m_fK_Burn			= pSettings->r_float("weapon","burn_immunity");
	m_fK_Strike			= pSettings->r_float("weapon","strike_immunity");
	m_fK_Shock			= pSettings->r_float("weapon","shock_immunity");
	m_fK_Wound			= pSettings->r_float("weapon","wound_immunity");
	m_fK_Radiation		= pSettings->r_float("weapon","radiation_immunity");
	m_fK_Telepatic		= pSettings->r_float("weapon","telepatic_immunity");
	m_fK_ChemicalBurn	= pSettings->r_float("weapon","chemical_burn_immunity");
	m_fK_Explosion		= pSettings->r_float("weapon","explosion_immunity");
	m_fK_FireWound		= pSettings->r_float("weapon","fire_wound_immunity");
	
	inherited::Load		(section);

	Fvector				pos,ypr;
	pos					= pSettings->r_fvector3		(section,"position");
	ypr					= pSettings->r_fvector3		(section,"orientation");
	ypr.mul				(PI/180.f);

	m_Offset.setHPB			(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over	(pos);

	fTimeToFire			= pSettings->r_float		(section,"rpm");
	fTimeToFire			= 60 / fTimeToFire;

	LPCSTR	name		= pSettings->r_string		(section,"wm_name");
	pstrWallmark		= name;
	fWallmarkSize		= pSettings->r_float		(section,"wm_size");

	LPCSTR hud_sect		= pSettings->r_string		(section,"hud");
	m_pHUD->Load		(hud_sect);

	// load ammo classes
	m_ammoTypes.clear	(); 
	LPCSTR				S = pSettings->r_string(section,"ammo_class");
	if (S && S[0]) 
	{
		string128		_ammoItem;
		int				count		= _GetItemCount	(S);
		for (int it=0; it<count; it++)	
		{
			_GetItem				(S,it,_ammoItem);
			m_ammoTypes.push_back	(_ammoItem);
		}
		m_ammoName = pSettings->r_string(*m_ammoTypes[0],"inv_name_short");
	}
	else
		m_ammoName = 0;

	m_resource = m_abrasion = pSettings->r_float(section,"resource");

	iAmmoElapsed		= pSettings->r_s32		(section,"ammo_elapsed"		);
	iMagazineSize		= pSettings->r_s32		(section,"ammo_mag_size"	);
	
	fireDistance		= pSettings->r_float		(section,"fire_distance"	);
	fireDispersionBase	= pSettings->r_float		(section,"fire_dispersion_base"	);	
	fireDispersionBase	= deg2rad(fireDispersionBase);
	fireDispersion		= pSettings->r_float		(section,"fire_dispersion"	);		
	fireDispersion		= deg2rad(fireDispersion);
	fireDispersion_Inc	= pSettings->r_float		(section,"fire_dispersion_add"); 
	fireDispersion_Dec	= pSettings->r_float		(section,"fire_dispersion_relax"); 

	fireDispersionConditionFactor = pSettings->r_float(section,"fire_dispersion_condition_factor"); 
	misfireProbability			  = pSettings->r_float(section,"misfire_probability"); 
	conditionDecreasePerShot	  = pSettings->r_float(section,"condition_shot_dec"); 

	fireDispersion_Current	= 0;

	camMaxAngle			= pSettings->r_float		(section,"cam_max_angle"	); camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed		= pSettings->r_float		(section,"cam_relax_speed"	); camRelaxSpeed = deg2rad(camRelaxSpeed);
	camDispersion		= pSettings->r_float		(section,"cam_dispersion"	); camDispersion = deg2rad(camDispersion);/**/

	dispVelFactor		= pSettings->r_float		(section,"disp_vel_factor"	);
	dispJumpFactor		= pSettings->r_float		(section,"disp_jump_factor"	);
	dispCrouchFactor	= pSettings->r_float		(section,"disp_crouch_factor");

	// tracer
	tracerHeadSpeed		= pSettings->r_float		(section,"tracer_head_speed"	);
	tracerTrailCoeff	= pSettings->r_float		(section,"tracer_trail_scale"	);
	tracerStartLength	= pSettings->r_float		(section,"tracer_start_length"	);
	tracerWidth			= pSettings->r_float		(section,"tracer_width"			);

	// light
	if(m_shotLight) 
	{
		Fvector clr			= pSettings->r_fvector3		(section,"light_color"		);
		light_base_color.set(clr.x,clr.y,clr.z,1);
		light_base_range	= pSettings->r_float		(section,"light_range"		);
		light_var_color		= pSettings->r_float		(section,"light_var_color"	);
		light_var_range		= pSettings->r_float		(section,"light_var_range"	);
		light_lifetime		= pSettings->r_float		(section,"light_time"		);
		light_time			= -1.f;
	}

	iHitPower			= pSettings->r_s32		(section,"hit_power"		);
	if(pSettings->line_exist(section,"hit_impulse")) fHitImpulse = pSettings->r_float(section,"hit_impulse");
	else fHitImpulse = 1.f;

	vFirePoint			= pSettings->r_fvector3		(section,"fire_point"		);
	vShellPoint			= pSettings->r_fvector3		(section,"shell_point"		);

	// flames
	iFlameDiv			= pSettings->r_s32			(section,"flame_div"		);
	fFlameLength		= pSettings->r_float		(section,"flame_length"		);
	fFlameSize			= pSettings->r_float		(section,"flame_size"		);

	// hands
	eHandDependence		= EHandDependence(pSettings->r_s32(section,"hand_dependence"));

	// slot
	iSlotBinding = m_slot = pSettings->r_s32		(section,"slot");

	m_fMinRadius		= pSettings->r_float		(section,"min_radius");
	m_fMaxRadius		= pSettings->r_float		(section,"max_radius");


	m_eScopeStatus			 = (CSE_ALifeItemWeapon::EAddonStatus)pSettings->r_s32(section,"scope_status");
	m_eSilencerStatus		 = (CSE_ALifeItemWeapon::EAddonStatus)pSettings->r_s32(section,"silencer_status");
	m_eGrenadeLauncherStatus = (CSE_ALifeItemWeapon::EAddonStatus)pSettings->r_s32(section,"grenade_launcher_status");

	
	if(m_eScopeStatus == CSE_ALifeItemWeapon::eAddondAttachable)
						m_sScopeName = pSettings->r_string(section,"scope_name");
	if(m_eSilencerStatus == CSE_ALifeItemWeapon::eAddondAttachable)
						m_sSilencerName = pSettings->r_string(section,"silencer_name");
	if(m_eGrenadeLauncherStatus == CSE_ALifeItemWeapon::eAddondAttachable)
						m_sGrenadeLauncherName = pSettings->r_string(section,"grenade_launcher_name");
}

BOOL CWeapon::net_Spawn		(LPVOID DC)
{
	BOOL bResult					= inherited::net_Spawn	(DC);
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemWeapon			    *E	= dynamic_cast<CSE_ALifeItemWeapon*>(e);

	//iAmmoCurrent					= E->a_current;
	iAmmoElapsed					= E->a_elapsed;
	m_flagsAddOnState				= E->m_addon_flags.get();

	if(iAmmoElapsed) 
	{
		CCartridge l_cartridge; 
		l_cartridge.Load(*m_ammoTypes[m_ammoType]);
		for(int i = 0; i < iAmmoElapsed; i++) 
			m_magazine.push(l_cartridge);
	}
	
	//if(Local()) OnStateSwitch					(E->state);
	//STATE = NEXT_STATE = E->state;

	setVisible						(true);
	setEnabled						(true);

	ShaderCreate				(hUIIcon,"hud\\default","");

	if (0==pstrWallmark)		hWallmark			= 0; 
	else						hWallmark.create	("effects\\wallmark",*pstrWallmark);

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		E->add_Box							(obb);

		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setDensity			(500.f);//400 - ieioiinou o.a. - ianna 1 i^3!
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate			(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
	}

	return bResult;
}

void CWeapon::net_Destroy	()
{
	inherited::net_Destroy	();

	if (m_pPhysicsShell)	m_pPhysicsShell->Deactivate	();
	xr_delete				(m_pPhysicsShell);

	hUIIcon.destroy			();
	hWallmark.destroy		();
}

void CWeapon::net_Export	(NET_Packet& P)
{
	u8 flags				=	u8(IsUpdating()? M_UPDATE_WEAPON_wfVisible:0);
	flags					|=	u8(IsWorking() ? M_UPDATE_WEAPON_wfWorking:0);

//	CGameObject::net_Export(P);
	P.w_u32					(Level().timeServer());
	P.w_u8					(flags);

	P.w_u16					(u16(0/*iAmmoCurrent*/));
	P.w_u16					(u16(0/*iAmmoElapsed*/));

	//////
	Fvector debug_vector = XFORM().c;
	P.w_vec3				(debug_vector);
	//P.w_vec3				(XFORM().c);

	float					_x,_y,_z;
	XFORM().getHPB			(_y,_x,_z);
	P.w_angle8				(_x);
	P.w_angle8				(_y);
	P.w_angle8				(_z);

	P.w_u8					(m_flagsAddOnState);
}

void CWeapon::net_Import	(NET_Packet& P)
{
	net_update				N;

//	CGameObject::net_Import(P);
	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(N.flags);

	P.r_u16					(N.ammo_current);
	P.r_u16					(N.ammo_elapsed);

	P.r_vec3				(N.pos);
	P.r_angle8				(N.angles.x);
	P.r_angle8				(N.angles.y);
	P.r_angle8				(N.angles.z);

	P.r_u8					(N.m_flagsAddOnState);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	
	{
		NET.push_back			(N);
	}
}

void CWeapon::shedule_Update	(u32 dT)
{
	// Queue shrink
	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	// Inherited
	inherited::shedule_Update	(dT);
}

void CWeapon::OnH_B_Independent	()
{
	inherited::OnH_B_Independent();

	if(m_pHUD)					m_pHUD->Hide();

	setVisible					(true);
	setEnabled					(true);

	CWeapon::FireEnd();
	hud_mode					= FALSE;
	UpdateXForm					();
//	if (m_pPhysicsShell)		m_pPhysicsShell->Activate	(XFORM(),0,XFORM());
	if(m_pPhysicsShell) 
	{
		Fvector l_fw; l_fw.set(XFORM().k); l_fw.mul(2.f);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM());
		l_p2.set(XFORM()); l_fw.mul(2.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
		Position().set(XFORM().c);
	}
}

void CWeapon::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();

	setVisible					(false);
	setEnabled					(false);

	if(m_pHUD)					m_pHUD->Hide();

	if (m_pPhysicsShell)		m_pPhysicsShell->Deactivate	();

/*	if(Local()) 
	{
		OnStateSwitch(eShowing);
	}*/

	/*
	if (Local() && (0xffff!=respawnPhantom)) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_RESPAWN,respawnPhantom);
		u_EventSend		(P);
		respawnPhantom	= 0xffff;
	}
	*/

}

int CWeapon::Ammo_eject		()
{
	int		save = /*iAmmoCurrent+*/iAmmoElapsed; 
	/*iAmmoCurrent = */iAmmoElapsed = 0; 

	/*
	if (Local() && (0xffff!=respawnPhantom)) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_RESPAWN,respawnPhantom);
		u_EventSend		(P);
	}
	*/

	return	save;  
}

void CWeapon::Ammo_add(int iValue) 
{
	//SpawnAmmo();
	//iAmmoCurrent+=iValue;
}

void CWeapon::net_update::lerp(CWeapon::net_update& A, CWeapon::net_update& B, float f)
{
	float invf		= 1.f-f;
	flags			= (f<0.5f)?A.flags:B.flags;
	ammo_current	= u16(iFloor(invf*float(A.ammo_current)+f*float(B.ammo_current)));
	ammo_elapsed	= u16(iFloor(invf*float(A.ammo_elapsed)+f*float(B.ammo_elapsed)));
	pos.lerp		(A.pos,B.pos,f);
	angles.x		= angle_lerp	(A.angles.x,B.angles.x,	f);
	angles.y		= angle_lerp	(A.angles.y,B.angles.y,	f);
	angles.z		= angle_lerp	(A.angles.z,B.angles.z,	f);
}

void CWeapon::UpdateCL		()
{
	inherited::UpdateCL		();

	// Logic
	float dt				= Device.fTimeDelta;
	fireDispersion_Current	-=	fireDispersion_Dec*dt;
	clamp					(fireDispersion_Current,0.f,1.f);
	if (m_shotLight && light_time>0)		{
		light_time -= dt;
		if (light_time<=0)
			light_render->set_active(false);
	}

	if (Remote() && NET.size())
	{
		// distinguish interpolation/extrapolation
		u32	dwTime			= Level().timeServer()-NET_Latency;
		net_update&	N		= NET.back();
		if ((dwTime > N.dwTimeStamp) || (NET.size()<2))
		{
			// BAD.	extrapolation
			NET_Last		= N;
		} else {
			// OK.	interpolation

			// Search 2 keyframes for interpolation
			int select		= -1;
			for (u32 id=0; id<NET.size()-1; id++)
			{
				if ((NET[id].dwTimeStamp<=dwTime)&&(dwTime<=NET[id+1].dwTimeStamp))	select=id;
			}
			if (select>=0)		
			{
				// Interpolate state
				net_update&	A		= NET[select+0];
				net_update&	B		= NET[select+1];
				u32	d1				= dwTime-A.dwTimeStamp;
				u32	d2				= B.dwTimeStamp - A.dwTimeStamp;
				float	factor		= (float(d1)/float(d2));
				NET_Last.lerp		(A,B,factor);

				// 
				////iAmmoCurrent		= NET_Last.ammo_current;
				////iAmmoElapsed		= NET_Last.ammo_elapsed;
				if (NET_Last.flags&M_UPDATE_WEAPON_wfWorking)
				{
					if (!IsWorking())	{ FireStart(); }
				} else {
					if (IsWorking())	{ FireEnd(); }
				}
			}
		}
	}

	if (0==H_Parent() && m_pPhysicsShell)		
	{
		m_pPhysicsShell->Update	();
		XFORM().set			(m_pPhysicsShell->mXFORM);
		XFORM().set			(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}

void CWeapon::SwitchState(u32 S)
{
	//R_ASSERT(S <= eHidden);
	if (Local() && /*??????? (S!=STATE) ??????? &&*/ (S!=NEXT_STATE))	
	{
		// !!! Just single entry for given state !!!
		NEXT_STATE		= S;	// Very-very important line of code!!! :)
		NET_Packet		P;
		u_EventGen		(P,GE_WPN_STATE_CHANGE,ID());
		P.w_u8			(u8(S));
		u_EventSend		(P);
	}
}

void CWeapon::renderable_Render		()
{
	if (m_shotLight && light_time>0) 
	{
		UpdateFP	();
		Light_Render(vLastFP);
	}
	if (m_pHUD /*&& hud_mode*/)	
		PSkeletonAnimated(m_pHUD->Visual())->Update	();
}

void CWeapon::signal_HideComplete()
{
	if(H_Parent()) setVisible(FALSE);
	bPending = false;
	if(m_pHUD) m_pHUD->Hide();
}

void CWeapon::SetDefaults()
{
	bWorking		= false;
	bWorking2		= false;
	bPending		= false;
}

void CWeapon::UpdatePosition(const Fmatrix& trans)
{
	Position().set	(trans.c);
	XFORM().mul	(trans,m_Offset);
}

void CWeapon::FireShotmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	if (!hWallmark)	return;
	
	if (R.O) {
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
#pragma todo("Oles to Yura: replace 'CPSObject' with 'CParticlesObject'")
			/*
			IRender_Sector* S	= R.O->Sector();
			Fvector D;	D.invert(vDir);

			LPCSTR ps_gibs		= "blood_1";//(Random.randI(5)==0)?"sparks_1":"stones";
			CPSObject* PS		= xr_new<CPSObject> (ps_gibs,S,true);
			PS->m_Emitter.m_ConeDirection.set(D);
			PS->play_at_pos		(vEnd);
			*/
		}
	} else {
		::Render->add_Wallmark	(
			hWallmark,
			vEnd,
			fWallmarkSize,
			g_pGameLevel->ObjectSpace.GetStaticTris()+R.element);
	}
}

BOOL CWeapon::FireTrace		(const Fvector& P, const Fvector& Peff, Fvector& D)
{
	Collide::ray_query RQ;

	R_ASSERT(m_magazine.size());

	CCartridge &l_cartridge = m_magazine.top();

	// direct it by dispersion factor
	Fvector dir, dir1;
	//dir.random_dir			(D,(fireDispersionBase+fireDispersion*fireDispersion_Current)*GetPrecision(),Random);
	dir.random_dir(D,(fireDispersion*fireDispersion_Current)*
					  GetPrecision()*
					  GetConditionDispersionFactor(),Random);

	// increase dispersion
	fireDispersion_Current += fireDispersion_Inc;
	clamp(fireDispersion_Current,0.f,1.f);

	//�������� ������������ ������
	ChangeCondition(-conditionDecreasePerShot);
	

	BOOL bResult = false;

	for(int i = 0; i < l_cartridge.m_buckShot; i++) 
	{
		dir1.random_dir(dir, fireDispersionBase * l_cartridge.m_kDisp, Random);

		// ...and trace line
		H_Parent()->setEnabled(false);
		bResult |= Level().ObjectSpace.RayPick(P, dir, fireDistance*l_cartridge.m_kDist, RQ);
		H_Parent()->setEnabled(true);
		D = dir1;

		// ...analyze
		Fvector end_point; 
		end_point.mad(P,D,RQ.range);
		if(bResult) 
		{
			if(Local() && RQ.O) 
			{
				float power = float(iHitPower) * l_cartridge.m_kHit;
				float scale = 1-(RQ.range/(fireDistance*l_cartridge.m_kDist)); clamp(scale,0.f,1.f); scale = _sqrt(scale);
				power *= scale;
				float impulse = fHitImpulse * l_cartridge.m_kImpulse * scale;
				CEntity* E = dynamic_cast<CEntity*>(RQ.O);
				//CGameObject* GO	=	dynamic_cast<CGameObject*>(RQ.O);
				if(E) power *= E->HitScale(RQ.element);

				// object-space
				Fvector p_in_object_space,position_in_bone_space;
				Fmatrix m_inv;
				m_inv.invert(RQ.O->XFORM());
				m_inv.transform_tiny(p_in_object_space, end_point);

				// bone-space
				CKinematics* V = PKinematics(RQ.O->Visual());
				
				if(V)
				{
					Fmatrix& m_bone = (V->LL_GetBoneInstance(u16(RQ.element))).mTransform;
					Fmatrix  m_inv_bone;
					m_inv_bone.invert(m_bone);
					m_inv_bone.transform_tiny(position_in_bone_space, p_in_object_space);
				}
				else
				{
					position_in_bone_space.set(p_in_object_space);
				}

				//  
				NET_Packet		P;
				u_EventGen		(P,GE_HIT,RQ.O->ID());
				P.w_u16			(u16(H_Parent()->ID()));
				P.w_dir			(D);
				P.w_float		(power);
				P.w_s16			((s16)RQ.element);
				P.w_vec3		(position_in_bone_space);
				P.w_float		(impulse);
				P.w_u16			(eHitTypeWound);
				u_EventSend		(P);
			}
			FireShotmark(D, end_point, RQ);
		}

		// tracer
		if(l_cartridge.m_tracer && tracerFrame != Device.dwFrame) 
		{
			tracerFrame = Device.dwFrame;
			Level().Tracers.Add	(Peff,end_point,tracerHeadSpeed,tracerTrailCoeff,tracerStartLength,tracerWidth);
		}
	}

	// light
	if(m_shotLight) Light_Start();
	
	// Ammo
	if(Local()) 
	{
		m_abrasion		= _max(0.f, m_abrasion - l_cartridge.m_impair);
		m_magazine.pop	();
		if(!(--iAmmoElapsed)) OnMagazineEmpty();

		//��������� �� ��������� �� ������
		CheckForMisfire();
	}
	
	return				bResult;
}

void CWeapon::Light_Start	()
{
	if (Device.dwFrame	!= light_frame)
	{
		light_frame					= Device.dwFrame;
		light_time					= light_lifetime;
		
		light_build_color.set		(Random.randFs(light_var_color,light_base_color.r),Random.randFs(light_var_color,light_base_color.g),Random.randFs(light_var_color,light_base_color.b),1);
		light_build_range			= Random.randFs(light_var_range,light_base_range);

		light_render->set_active	(true);
	}
}

void CWeapon::Light_Render	(Fvector& P)
{
	float light_scale			= light_time/light_lifetime;
	light_render->set_position	(P);
	light_render->set_color		(light_build_color.r*light_scale,light_build_color.g*light_scale,light_build_color.b*light_scale);
	light_render->set_range		(light_build_range*light_scale);
}

//���������� ������� ������� �� ������
void CWeapon::OnDrawFlame	()
{
	if (fFlameTime>0)	
	{
#pragma todo("Oles to Yura: replace '::Render->add_Patch' with particles")
		/*
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
		*/

		
//		CParticlesObject* pStaticPG;
//		pStaticPG = xr_new<CParticlesObject>("weapons\\generic_flame",Sector());
//		Fmatrix pos; 
//		pos.set(XFORM()); 
//		pos.c.set(vLastFP);
//		Fvector vel; 
//		vel.sub(Position(),ps_Element(0).vPosition); 
//		vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);
//		pStaticPG->UpdateParent(pos, vel); 
//		pStaticPG->Play();


		fFlameTime -= Device.fTimeDelta;
	}
}

void CWeapon::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);
	switch (type)
	{
	case GE_WPN_STATE_CHANGE:
		{
			u8				S;
			P.r_u8			(S);
			OnStateSwitch	(u32(S));
		}
		break;
	case GE_WPN_AMMO_ADD:
		{
			u16 amount;
			P.r_u16			(amount);
			Ammo_add		(amount);
		}
		break;
	}
}

bool CWeapon::Activate() 
{
	Show();
	return true;
}

void CWeapon::Deactivate() 
{
	Hide();
}

bool CWeapon::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	if(bPending) return false;
	
	switch(cmd) 
	{
		case kWPN_FIRE: 
			{
                if(flags&CMD_START) 
					FireStart();
				else 
					FireEnd();
			} 
			return true;
		case kWPN_NEXT: 
			{
				if(flags&CMD_START) 
				{
					u32 l_newType = m_ammoType;
					do 
					{
						l_newType = (l_newType+1)%m_ammoTypes.size();
					} while(l_newType != m_ammoType && 
							!m_pInventory->Get(*m_ammoTypes[l_newType],
												!dynamic_cast<CActor*>(H_Parent())));
				
					if(l_newType != m_ammoType) 
					{
						m_ammoType = l_newType;
						m_pAmmo = NULL;
						Reload();
					}
				}
			} 
            return true;
	}
	return false;
}

void CWeapon::SpawnAmmo(u32 boxCurr, LPCSTR ammoSect) 
{
	if(!m_ammoTypes.size()) return;
	
	static l_type = 0; 
	l_type %= m_ammoTypes.size();

	if(!ammoSect) 
		ammoSect = *m_ammoTypes[l_type/*m_ammoType*/]; 
	//m_ammoType++; m_ammoType %= m_ammoTypes.size();
	
	l_type++; 
	l_type %= m_ammoTypes.size();

	// Create
	CSE_Abstract		*D		= F_entity_Create(ammoSect);
	CSE_ALifeItemAmmo	*l_pA	= dynamic_cast<CSE_ALifeItemAmmo*>(D);
	R_ASSERT(l_pA);
	// Fill
	l_pA->m_boxSize = (u16)pSettings->r_s32(ammoSect, "box_size");
	strcpy(D->s_name, ammoSect);
	strcpy(D->s_name_replace, "");
	D->s_gameid = u8(GameID());
	D->s_RP = 0xff;
	D->ID = 0xffff;
	D->ID_Parent = (u16)H_Parent()->ID();
	D->ID_Phantom = 0xffff;
	D->s_flags.set(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime = 0;
	l_pA->m_tNodeID	= AI_NodeID;
	
	// Send
	if(boxCurr == 0xffffffff) boxCurr = l_pA->m_boxSize;
	
	while(boxCurr) 
	{
		l_pA->a_elapsed = (u16)(boxCurr > l_pA->m_boxSize ? l_pA->m_boxSize : boxCurr);
		NET_Packet P;
		D->Spawn_Write(P, TRUE);
		Level().Send(P,net_flags(TRUE));
		if(boxCurr > l_pA->m_boxSize) boxCurr -= l_pA->m_boxSize;
		else boxCurr = 0;
	}
	// Destroy
	F_entity_Destroy(D);
}

const char* CWeapon::Name() 
{
	if(m_name) 
		strcpy(m_tmpName, m_name); 
	else m_tmpName[0] = 0;
	
	char l_tmp[20]; 
	sprintf(l_tmp, " %d/%d %s", iAmmoElapsed, iMagazineSize, m_ammoName);
	strcpy(&m_tmpName[xr_strlen(m_tmpName)], l_tmp);
	return m_tmpName;
}

int CWeapon::GetAmmoCurrent() 
{
	int l_count = iAmmoElapsed;
	if(!m_pInventory) return l_count;

	for(int i = 0; i < (int)m_ammoTypes.size(); i++) 
	{
		LPCSTR l_ammoType = *m_ammoTypes[i];
//		if(dynamic_cast<CActor*>(H_Parent())) 
		{
			TIItemList &l_list = m_pInventory->m_belt;
			for(PPIItem l_it = l_list.begin(); l_it != l_list.end(); l_it++) 
			{
				CWeaponAmmo *l_pAmmo = dynamic_cast<CWeaponAmmo*>(*l_it);
				
				if(l_pAmmo && !strcmp(l_pAmmo->cNameSect(), l_ammoType)) 
				{
					l_count = l_count + l_pAmmo->m_boxCurr;
				}
			}
		}
		
		TIItemList &l_list = m_pInventory->m_ruck;
		for(PPIItem l_it = l_list.begin(); l_it != l_list.end(); l_it++) 
		{
			CWeaponAmmo *l_pAmmo = dynamic_cast<CWeaponAmmo*>(*l_it);
			if(l_pAmmo && !strcmp(l_pAmmo->cNameSect(), l_ammoType)) 
			{
				l_count = l_count + l_pAmmo->m_boxCurr;
			}
		}
	}
	return l_count;
}


float CWeapon::GetConditionDispersionFactor()
{
	return fireDispersionConditionFactor*GetCondition();
}
float CWeapon::GetConditionMisfireProbability()
{
	return misfireProbability*(1.f-GetCondition());
}

BOOL CWeapon::CheckForMisfire()
{
	float rnd = ::Random.randF(0.f,1.f);
	if(rnd<GetConditionMisfireProbability())
	{
		FireEnd();

		bMisfire = true;
		SwitchState(eMisfire);
		
		if(dynamic_cast<CActor*>(this->H_Parent()))
			HUD().outMessage(0xffffffff,this->cName(), "gun jammed");
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CWeapon::IsMisfire()
{	
	return bMisfire;
}
void CWeapon::Reload()
{
}

bool CWeapon::Attach(PIItem pIItem) 
{
	return inherited::Attach(pIItem);
}
bool CWeapon::Detach(const char* item_section_name)
{
	return inherited::Detach(item_section_name);
}

bool CWeapon::IsGreandeLauncherAttached()
{
	return (m_eGrenadeLauncherStatus == CSE_ALifeItemWeapon::eAddondAttachable &&
			(m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) != 0);
}
bool CWeapon::IsScopeAttached()
{
	return (m_eScopeStatus == CSE_ALifeItemWeapon::eAddondAttachable &&
			(m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) != 0);

}
bool CWeapon::IsSilencerAttached()
{
	return (m_eSilencerStatus == CSE_ALifeItemWeapon::eAddondAttachable &&
			(m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) != 0);
}