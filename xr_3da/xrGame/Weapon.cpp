// Weapon.cpp: implementation of the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Weapon.h"
#include "ParticlesObject.h"
#include "HUDManager.h"
#include "WeaponHUD.h"
#include "entity_alive.h"

#include "inventory.h"
#include "xrserver_objects_alife_items.h"

#include "actor.h"
#include "actoreffector.h"
#include "level.h"

#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "../skeletoncustom.h"

#define WEAPON_REMOVE_TIME		30000
#define ROTATION_TIME			0.25f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeapon::CWeapon(LPCSTR name)
{
	STATE				= NEXT_STATE		= eHidden;

	SetDefaults			();

	m_Offset.identity	();

	vLastFP.set			(0,0,0);
	vLastFP2.set		(0,0,0);
	vLastFD.set			(0,0,0);
	vLastSP.set			(0,0,0);

	iAmmoCurrent		= -1;
	m_dwAmmoCurrentCalcFrame = 0;

	iAmmoElapsed		= -1;
	iMagazineSize		= -1;
	iBuckShot			= 1;
	m_ammoType			= 0;
	m_ammoName			= NULL;

	eHandDependence		= hdNone;

	m_fZoomFactor		= DEFAULT_FOV;
	m_fZoomRotationFactor = 0.f;


	m_pAmmo				= NULL;


	m_pFlameParticles2	= NULL;
	m_sFlameParticles2 = NULL;


	m_fCurrentCartirdgeDisp = 1.f;

}

CWeapon::~CWeapon		()
{
}

void CWeapon::Hit(float P, Fvector &dir,	
		    CObject* who, s16 element,
		    Fvector position_in_object_space, 
		    float impulse, 
		    ALife::EHitType hit_type)
{
	inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);
}



void CWeapon::UpdateXForm	()
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (0==H_Parent())	return;

		// Get access to entity and its visual
		CEntityAlive*	E		= smart_cast<CEntityAlive*>(H_Parent());
		
		if(!E) return;
		R_ASSERT		(E);
		CKinematics*	V		= smart_cast<CKinematics*>	(E->Visual());
		VERIFY			(V);

		// Get matrices
		int				boneL,boneR,boneR2;
		E->g_WeaponBones(boneL,boneR,boneR2);
		if ((HandDependence() == hd1Hand) || (STATE == eReload) || (!E->g_Alive()))
			boneL = boneR2;
#pragma todo("TO ALL: serious performance problem")
		V->CalculateBones	();
		Fmatrix& mL			= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR			= V->LL_GetTransform(u16(boneR));
		// Calculate
		Fmatrix				mRes;
		Fvector				R,D,N;
		D.sub				(mL.c,mR.c);	

		if(fis_zero(D.magnitude()))
		{
			mRes.set(E->XFORM());
			mRes.c.set(mR.c);
		}
		else
		{		
			D.normalize();
			R.crossproduct	(mR.j,D);

			N.crossproduct	(D,R);			
			N.normalize();

			mRes.set		(R,N,D,mR.c);
			mRes.mulA_43	(E->XFORM());
		}

		UpdatePosition	(mRes);
	}
}

void CWeapon::UpdateFP		()
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm			();

		if (hud_mode && (0!=H_Parent()))// && Local())
		{
			// 1st person view - skeletoned
			CKinematics* V			= smart_cast<CKinematics*>(m_pHUD->Visual());
			V->CalculateBones		();

			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(u16(m_pHUD->FireBone()));
			Fmatrix& parent			= m_pHUD->Transform	();

			Fvector& fp				= m_pHUD->FirePoint();
			Fvector& fp2			= m_pHUD->FirePoint2();
			Fvector& sp				= m_pHUD->ShellPoint();

			fire_mat.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastFP);
			fire_mat.transform_tiny	(vLastFP2,fp2);
			parent.transform_tiny	(vLastFP2);
		
			fire_mat.transform_tiny	(vLastSP,sp);
			parent.transform_tiny	(vLastSP);

			vLastSD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastSD);

			
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);

			m_FireParticlesXForm.identity();
			m_FireParticlesXForm.k.set(vLastFD);
			Fvector::generate_orthonormal_basis(m_FireParticlesXForm.k,
									m_FireParticlesXForm.j, m_FireParticlesXForm.i);
		} else {
			// 3rd person or no parent
			Fmatrix& parent			= XFORM();
			Fvector& fp				= vFirePoint;
			Fvector& fp2			= vFirePoint2;
			Fvector& sp				= vShellPoint;

			parent.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastFP2,fp2);
			parent.transform_tiny	(vLastSP,sp);
			
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);

			//vLastSD = sd;
			vLastSD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastSD);

			m_FireParticlesXForm.set(parent);
		}
	}
}

void CWeapon::Load		(LPCSTR section)
{
	// verify class
	LPCSTR Class		= pSettings->r_string		(section,"class");
	CLASS_ID load_cls	= TEXT2CLSID(Class);
	R_ASSERT			(load_cls==SUB_CLS_ID);

	inherited::Load					(section);
	CShootingObject::Load			(section);

	
	if(pSettings->line_exist(section, "flame_particles_2"))
		m_sFlameParticles2 = pSettings->r_string(section, "flame_particles_2");


	Fvector				pos,ypr;
	pos					= pSettings->r_fvector3		(section,"position");
	ypr					= pSettings->r_fvector3		(section,"orientation");
	ypr.mul				(PI/180.f);

	m_Offset.setHPB			(ypr.x,ypr.y,ypr.z);
	m_Offset.translate_over	(pos);

	// load ammo classes
	m_ammoTypes.clear	(); 
	LPCSTR				S = pSettings->r_string(section,"ammo_class");
	if (S && S[0]) 
	{
		string128		_ammoItem;
		int				count		= _GetItemCount	(S);
		for (int it=0; it<count; ++it)	
		{
			_GetItem				(S,it,_ammoItem);
			m_ammoTypes.push_back	(_ammoItem);
		}
		m_ammoName = pSettings->r_string(*m_ammoTypes[0],"inv_name_short");
	}
	else
		m_ammoName = 0;

	iAmmoElapsed		= pSettings->r_s32		(section,"ammo_elapsed"		);
	iMagazineSize		= pSettings->r_s32		(section,"ammo_mag_size"	);
	
	////////////////////////////////////////////////////
	// дисперсия стрельбы

	//подбрасывание камеры во время отдачи
	camMaxAngle			= pSettings->r_float		(section,"cam_max_angle"	); 
	camMaxAngle			= deg2rad					(camMaxAngle);
	camRelaxSpeed		= pSettings->r_float		(section,"cam_relax_speed"	); 
	camRelaxSpeed		= deg2rad					(camRelaxSpeed);
	camDispersion		= pSettings->r_float		(section,"cam_dispersion"	); 
	camDispersion		= deg2rad					(camDispersion);

	camMaxAngleHorz		= pSettings->r_float		(section,"cam_max_angle_horz"	); 
	camMaxAngleHorz		= deg2rad					(camMaxAngleHorz);
	camStepAngleHorz	= pSettings->r_float		(section,"cam_step_angle_horz"	); 
	camStepAngleHorz	= deg2rad					(camStepAngleHorz);

	fireDispersionConditionFactor = pSettings->r_float(section,"fire_dispersion_condition_factor"); 
	misfireProbability			  = pSettings->r_float(section,"misfire_probability"); 
	conditionDecreasePerShot	  = pSettings->r_float(section,"condition_shot_dec"); 

		
	vFirePoint			= pSettings->r_fvector3		(section,"fire_point"		);
	
	if(pSettings->line_exist(section,"fire_point2")) 
		vFirePoint2 = pSettings->r_fvector3(section,"fire_point2");
	else 
		vFirePoint2 = vFirePoint;

	
	// hands
	eHandDependence		= EHandDependence(pSettings->r_s32(section,"hand_dependence"));
	// 
	m_fMinRadius		= pSettings->r_float		(section,"min_radius");
	m_fMaxRadius		= pSettings->r_float		(section,"max_radius");


	// информация о возможных апгрейдах и их визуализации в инвентаре
	m_eScopeStatus			 = (ALife::EWeaponAddonStatus)pSettings->r_s32(section,"scope_status");
	m_eSilencerStatus		 = (ALife::EWeaponAddonStatus)pSettings->r_s32(section,"silencer_status");
	m_eGrenadeLauncherStatus = (ALife::EWeaponAddonStatus)pSettings->r_s32(section,"grenade_launcher_status");

	m_bZoomEnabled = !!pSettings->r_bool(section,"zoom_enabled");
	m_fZoomRotateTime = ROTATION_TIME;
	if(m_bZoomEnabled && m_pHUD) LoadZoomOffset(*hud_sect, "");

	if(m_eScopeStatus == ALife::eAddonAttachable)
	{
		m_sScopeName = pSettings->r_string(section,"scope_name");
		m_iScopeX = pSettings->r_s32(section,"scope_x");
		m_iScopeY = pSettings->r_s32(section,"scope_y");
	}

    
	if(m_eSilencerStatus == ALife::eAddonAttachable)
	{
		m_sSilencerName = pSettings->r_string(section,"silencer_name");
		m_iSilencerX = pSettings->r_s32(section,"silencer_x");
		m_iSilencerY = pSettings->r_s32(section,"silencer_y");
	}

    
	if(m_eGrenadeLauncherStatus == ALife::eAddonAttachable)
	{
		m_sGrenadeLauncherName = pSettings->r_string(section,"grenade_launcher_name");
		m_iGrenadeLauncherX = pSettings->r_s32(section,"grenade_launcher_x");
		m_iGrenadeLauncherY = pSettings->r_s32(section,"grenade_launcher_y");
	}

	InitAddons();

	//////////////////////////////////////
	//время убирания оружия с уровня
	if(pSettings->line_exist(section,"weapon_remove_time"))
		m_dwWeaponRemoveTime = pSettings->r_u32(section,"weapon_remove_time");
	else
		m_dwWeaponRemoveTime = WEAPON_REMOVE_TIME;
	//////////////////////////////////////
	if(pSettings->line_exist(section,"auto_spawn_ammo"))
		m_bAutoSpawnAmmo = pSettings->r_bool(section,"auto_spawn_ammo");
	else
		m_bAutoSpawnAmmo = TRUE;
	//////////////////////////////////////


	m_bHideCrosshairInZoom = true;
	if(pSettings->line_exist(hud_sect, "zoom_hide_crosshair"))
		m_bHideCrosshairInZoom = !!pSettings->r_bool(hud_sect, "zoom_hide_crosshair");
}


void CWeapon::LoadZoomOffset (LPCSTR section, LPCSTR prefix)
{
	string256 full_name;
	m_pHUD->SetZoomOffset(pSettings->r_fvector3	(hud_sect, strconcat(full_name, prefix, "zoom_offset")));
	m_pHUD->SetZoomRotateX(pSettings->r_float	(hud_sect, strconcat(full_name, prefix, "zoom_rotate_x")));
	m_pHUD->SetZoomRotateY(pSettings->r_float	(hud_sect, strconcat(full_name, prefix, "zoom_rotate_y")));

	if(pSettings->line_exist(hud_sect, "zoom_rotate_time"))
		m_fZoomRotateTime = pSettings->r_float(hud_sect,"zoom_rotate_time");
}

void CWeapon::animGet	(MotionSVec& lst, LPCSTR prefix)
{
	CMotionDef* M		= m_pHUD->animGet(prefix);
	if (M)				lst.push_back(M);
	for (int i=0; i<MAX_ANIM_COUNT; ++i)
	{
		string128		sh_anim;
		sprintf			(sh_anim,"%s%d",prefix,i);
		M				= m_pHUD->animGet(sh_anim);
		if (M)			lst.push_back(M);
	}
	R_ASSERT2(!lst.empty(),prefix);
}


BOOL CWeapon::net_Spawn		(LPVOID DC)
{
	BOOL bResult					= inherited::net_Spawn(DC);
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemWeapon			    *E	= smart_cast<CSE_ALifeItemWeapon*>(e);

	//iAmmoCurrent					= E->a_current;
	iAmmoElapsed					= E->a_elapsed;
	m_flagsAddOnState				= E->m_addon_flags.get();
	m_ammoType						= E->ammo_type;
	STATE = NEXT_STATE				= E->state;
	
	if(iAmmoElapsed) 
	{
		CCartridge l_cartridge; 
		l_cartridge.Load(*m_ammoTypes[m_ammoType]);
		m_fCurrentCartirdgeDisp = l_cartridge.m_kDisp;
		for(int i = 0; i < iAmmoElapsed; ++i) 
			m_magazine.push(l_cartridge);
	}
	
	Light_Create		();

	UpdateAddonsVisibility();
	InitAddons();


	m_dwWeaponIndependencyTime = 0;

	VERIFY((u32)iAmmoElapsed == m_magazine.size());

	return bResult;
}

void CWeapon::net_Destroy	()
{
	inherited::net_Destroy	();

	//удалить объекты партиклов
	StopFlameParticles	();
	StopFlameParticles2	();
	StopLight			();
	Light_Destroy		();

	while (m_magazine.size()) m_magazine.pop();
}

void CWeapon::net_Export	(NET_Packet& P)
{
	inherited::net_Export (P);

	u8 flags				=	u8(IsUpdating()? M_UPDATE_WEAPON_wfVisible:0);
	flags					|=	u8(IsWorking() ? M_UPDATE_WEAPON_wfWorking:0);

	P.w_u8					(flags);

	P.w_u16					(u16(iAmmoElapsed));

	P.w_u8					(m_flagsAddOnState);
	P.w_u8					((u8)m_ammoType);
	P.w_u8					((u8)STATE);

	P.w_u8					((u8)m_bZoomMode);
}

void CWeapon::net_Import	(NET_Packet& P)
{
	inherited::net_Import (P);

	u8 flags = 0;
	P.r_u8					(flags);

	u16 ammo_elapsed = 0;
	P.r_u16					(ammo_elapsed);
	u8 NewAddonState;
//	P.r_u8					(m_flagsAddOnState);
	P.r_u8					(NewAddonState);

	m_flagsAddOnState = NewAddonState;
	UpdateAddonsVisibility();

	u8 ammoType, wstate;
	P.r_u8					(ammoType);
	P.r_u8					(wstate);

	u8 Zoom;
	P.r_u8					((u8)Zoom);

	if (H_Parent() && H_Parent()->Remote())
	{
		if (Zoom) OnZoomIn();
		else OnZoomOut();
	};

	m_ammoType = ammoType;
	SetAmmoElapsed(int(ammo_elapsed));

	/*
	if (iAmmoElapsed && m_magazine.empty())
	{
		CCartridge l_cartridge; 
		l_cartridge.Load(*m_ammoTypes[m_ammoType]);
		for(int i = 0; i < iAmmoElapsed; ++i) 
			m_magazine.push(l_cartridge);
	};

	*/
//	STATE = wstate;
	VERIFY((u32)iAmmoElapsed == m_magazine.size());
	
}


void CWeapon::save(NET_Packet &output_packet)
{
	inherited::save(output_packet);
}
void CWeapon::load(IReader &input_packet)
{
	inherited::load(input_packet);
}


void CWeapon::OnEvent				(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);

	switch (type)
	{
	case GE_ADDON_CHANGE:
		{
			P.r_u8					(m_flagsAddOnState);
			InitAddons();
			UpdateAddonsVisibility();
		}break;
	}
};

void CWeapon::shedule_Update	(u32 dT)
{
	// Queue shrink
//	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
//	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();	

	// Inherited
	inherited::shedule_Update	(dT);
}

void CWeapon::OnH_B_Independent	()
{
	inherited::OnH_B_Independent();

	if (m_pHUD)
		m_pHUD->Hide			();

	//завершить принудительно все процессы что шли
	FireEnd();
	m_bPending = false;
	SwitchState(eIdle);

	hud_mode					= FALSE;
	UpdateXForm					();

	RemoveShotEffector			();
}

void CWeapon::OnH_A_Independent	()
{
	m_dwWeaponIndependencyTime = Level().timeServer();
	inherited::OnH_A_Independent();
};

void CWeapon::OnH_A_Chield		()
{
	inherited::OnH_A_Chield		();

	UpdateAddonsVisibility		();
};

void CWeapon::OnActiveItem ()
{
	inherited::OnActiveItem();
	//если мы занружаемся и оружие было в руках
	STATE = NEXT_STATE = eIdle;
	if (m_pHUD) m_pHUD->Show();
}

void CWeapon::OnHiddenItem ()
{
	inherited::OnHiddenItem();
	if (m_pHUD)	m_pHUD->Hide ();
	STATE = NEXT_STATE = eHidden;
}


void CWeapon::OnH_B_Chield		()
{
	m_dwWeaponIndependencyTime = 0;
	inherited::OnH_B_Chield		();

	OnZoomOut					();
}


void CWeapon::UpdateCL		()
{
	inherited::UpdateCL		();

	UpdateFP();

	//подсветка от выстрела
	UpdateLight();

	//нарисовать партиклы
	UpdateFlameParticles();
	UpdateFlameParticles2();

	make_Interpolation();
}


void CWeapon::renderable_Render		()
{
	UpdateFP();
	
	//нарисовать подсветку
	RenderLight();	

	UpdateXForm();

	//если мы в режиме снайперки, то сам HUD рисовать не надо
	if(IsZoomed() && !IsRotatingToZoom() && ZoomTexture())
		m_bRenderHud = false;
	else
		m_bRenderHud = true;

	inherited::renderable_Render		();
}

void CWeapon::signal_HideComplete()
{
	if(H_Parent()) setVisible(FALSE);
	m_bPending = false;
	if(m_pHUD) m_pHUD->Hide();
}

void CWeapon::SetDefaults()
{
	bWorking2			= false;
	m_bPending			= false;

	m_bUsingCondition = true;
	bMisfire = false;
	m_flagsAddOnState = 0;
	m_bZoomMode = false;
}

void CWeapon::UpdatePosition(const Fmatrix& trans)
{
	Position().set	(trans.c);
	XFORM().mul	(trans,m_Offset);
	VERIFY							(!fis_zero(DET(renderable.xform)));
}


bool CWeapon::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	
	switch(cmd) 
	{
		case kWPN_FIRE: 
			{
				//если оружие чем-то занято, то ничего не делать
				if(IsPending()) return false;


	            if(flags&CMD_START) 
					FireStart();
				else 
					FireEnd();
			} 
			return true;
		case kWPN_NEXT: 
			{
				//если оружие чем-то занято, то ничего не делать
				if(IsPending()) return false;

					
				if(flags&CMD_START) 
				{
					u32 l_newType = m_ammoType;
					do 
					{
						l_newType = (l_newType+1)%m_ammoTypes.size();
					} while(l_newType != m_ammoType && 
							!m_pInventory->Get(*m_ammoTypes[l_newType],
												!smart_cast<CActor*>(H_Parent())));
				
					if(l_newType != m_ammoType) 
					{
						m_ammoType = l_newType;
						m_pAmmo = NULL;
						Reload();
					}
				}
			} 
            return true;
		case kWPN_ZOOM:
			if(IsZoomEnabled())
			{
                if(flags&CMD_START && !IsPending())
					OnZoomIn();
                else if(IsZoomed())
					OnZoomOut();
				return true;
			} 
			else 
				return false;
	}
	return false;
}

void CWeapon::SpawnAmmo(u32 boxCurr, LPCSTR ammoSect, u32 ParentID) 
{
	if(!m_ammoTypes.size()) return;
	
	int l_type = 0; 
	l_type %= m_ammoTypes.size();

	if(!ammoSect) 
		ammoSect = *m_ammoTypes[l_type/*m_ammoType*/]; 
	//++m_ammoType; m_ammoType %= m_ammoTypes.size();
	
	++l_type; 
	l_type %= m_ammoTypes.size();

	// Create
	CSE_Abstract		*D		= F_entity_Create(ammoSect);
	CSE_ALifeItemAmmo	*l_pA	= smart_cast<CSE_ALifeItemAmmo*>(D);
	R_ASSERT(l_pA);
	// Fill
	l_pA->m_boxSize = (u16)pSettings->r_s32(ammoSect, "box_size");
	D->s_name		= ammoSect;
	strcpy(D->s_name_replace, "");
	D->s_gameid = u8(GameID());
	D->s_RP = 0xff;
	D->ID = 0xffff;
	if (ParentID == 0xffffffff)
		D->ID_Parent = (u16)H_Parent()->ID();
	else
		D->ID_Parent = (u16)ParentID;
	D->ID_Phantom = 0xffff;
	D->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime = 0;
	l_pA->m_tNodeID	= level_vertex_id();
	
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

int CWeapon::GetAmmoCurrent() const
{
	int l_count = iAmmoElapsed;
	if(!m_pInventory) return l_count;

	//чтоб не делать лишних пересчетов
	if(m_pInventory->ModifyFrame()<=m_dwAmmoCurrentCalcFrame)
		return l_count + iAmmoCurrent;

 	m_dwAmmoCurrentCalcFrame = Device.dwFrame;
	iAmmoCurrent = 0;

	for(int i = 0; i < (int)m_ammoTypes.size(); ++i) 
	{
		LPCSTR l_ammoType = *m_ammoTypes[i];

		for(PPIItem l_it = m_pInventory->m_belt.begin(); m_pInventory->m_belt.end() != l_it; ++l_it) 
		{
			CWeaponAmmo *l_pAmmo = smart_cast<CWeaponAmmo*>(*l_it);

			if(l_pAmmo && !xr_strcmp(l_pAmmo->cNameSect(), l_ammoType)) 
			{
				iAmmoCurrent = iAmmoCurrent + l_pAmmo->m_boxCurr;
			}
		}

		for(PPIItem l_it = m_pInventory->m_ruck.begin(); m_pInventory->m_ruck.end() != l_it; ++l_it) 
		{
			CWeaponAmmo *l_pAmmo = smart_cast<CWeaponAmmo*>(*l_it);
			if(l_pAmmo && !xr_strcmp(l_pAmmo->cNameSect(), l_ammoType)) 
			{
				iAmmoCurrent = iAmmoCurrent + l_pAmmo->m_boxCurr;
			}
		}
	}
	return l_count + iAmmoCurrent;
}

float CWeapon::GetConditionMisfireProbability() const
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
		
		if(smart_cast<CActor*>(this->H_Parent()))
			HUD().outMessage(0xffffffff,this->cName(), "gun jammed");
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CWeapon::IsMisfire() const
{	
	return bMisfire;
}
void CWeapon::Reload()
{
	OnZoomOut();
}

bool CWeapon::Attach(PIItem pIItem) 
{
	return inherited::Attach(pIItem);
}
bool CWeapon::Detach(const char* item_section_name)
{
	return inherited::Detach(item_section_name);
}

bool CWeapon::IsGrenadeLauncherAttached()
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
			0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)) || 
			CSE_ALifeItemWeapon::eAddonPermanent == m_eGrenadeLauncherStatus;
}
bool CWeapon::IsScopeAttached()
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eScopeStatus &&
			0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope)) || 
			CSE_ALifeItemWeapon::eAddonPermanent == m_eScopeStatus;

}
bool CWeapon::IsSilencerAttached()
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eSilencerStatus &&
			0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer)) || 
			CSE_ALifeItemWeapon::eAddonPermanent == m_eSilencerStatus;
}

bool CWeapon::GrenadeLauncherAttachable()
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus);
}
bool CWeapon::ScopeAttachable()
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eScopeStatus);
}
bool CWeapon::SilencerAttachable()
{
	return (CSE_ALifeItemWeapon::eAddonAttachable == m_eSilencerStatus);
}

void CWeapon::UpdateAddonsVisibility()
{
	CKinematics* pHudVisual = smart_cast<CKinematics*>(m_pHUD->Visual());// R_ASSERT(pHudVisual);
	if (H_Parent() != Level().CurrentEntity()) pHudVisual = NULL;
	CKinematics* pWeaponVisual = smart_cast<CKinematics*>(Visual()); R_ASSERT(pWeaponVisual);
	
	if(ScopeAttachable())
	{
		if(IsScopeAttached())
		{
			if (pHudVisual) pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID("wpn_scope"),TRUE,TRUE);
			pWeaponVisual->LL_SetBoneVisible(pWeaponVisual->LL_BoneID("wpn_scope"),TRUE,TRUE);
		}
		else
		{
			if (pHudVisual) pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID("wpn_scope"),FALSE,TRUE);
			pWeaponVisual->LL_SetBoneVisible(pWeaponVisual->LL_BoneID("wpn_scope"),FALSE,TRUE);
		}
	}
	if(SilencerAttachable())
	{
		if(IsSilencerAttached())
		{
			if (pHudVisual) pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID("wpn_silencer"),TRUE,TRUE);
			pWeaponVisual->LL_SetBoneVisible(pWeaponVisual->LL_BoneID("wpn_silencer"),TRUE,TRUE);
		}
		else
		{
			if (pHudVisual) pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID("wpn_silencer"),FALSE,TRUE);
			pWeaponVisual->LL_SetBoneVisible(pWeaponVisual->LL_BoneID("wpn_silencer"),FALSE,TRUE);
		}
	}
	if(GrenadeLauncherAttachable())
	{
		if(IsGrenadeLauncherAttached())
		{
			//if (pHudVisual) pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID("wpn_launcher"),TRUE,TRUE);
			pWeaponVisual->LL_SetBoneVisible(pWeaponVisual->LL_BoneID("wpn_launcher"),TRUE,TRUE);
		}
		else
		{
			//if (pHudVisual) pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID("wpn_launcher"),FALSE,TRUE);
			pWeaponVisual->LL_SetBoneVisible(pWeaponVisual->LL_BoneID("wpn_launcher"),FALSE,TRUE);
		}
	}
}

void CWeapon::InitAddons()
{
}


void CWeapon::OnZoomIn()
{
	m_bZoomMode = true;
	m_fZoomFactor = m_fScopeZoomFactor;
}

void CWeapon::OnZoomOut()
{
	m_bZoomMode = false;
	m_fZoomFactor = DEFAULT_FOV;
}
CUIStaticItem* CWeapon::ZoomTexture()
{
	if(m_UIScope.GetShader()) 
		return &m_UIScope;
	else 
		return NULL;
}

void CWeapon::SwitchState(u32 S)
{
	inherited::SwitchState(S);
}

void CWeapon::OnMagazineEmpty	()
{
	VERIFY((u32)iAmmoElapsed == m_magazine.size());
}

void CWeapon::make_Interpolation ()
{
	inherited::make_Interpolation();
}

void CWeapon::PH_B_CrPr			()
{
	inherited::PH_B_CrPr		();
}

void CWeapon::PH_I_CrPr			()
{
	inherited::PH_I_CrPr		();
}

void CWeapon::PH_A_CrPr			()
{
	inherited::PH_A_CrPr		();
}

void CWeapon::reinit			()
{
	CShootingObject::reinit		();
	CHudItem::reinit			();
}

void CWeapon::reload			(LPCSTR section)
{
	CShootingObject::reload		(section);
	CHudItem::reload			(section);
}

void CWeapon::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

void CWeapon::activate_physic_shell()
{
	CPhysicsShellHolder::activate_physic_shell();
}

void CWeapon::setup_physic_shell()
{
	CPhysicsShellHolder::setup_physic_shell();
}

bool CWeapon::NeedToDestroyObject()	const
{
	if (GameID() == GAME_SINGLE) return false;
	if (Remote()) return false;
	if (TimePassedAfterIndependant() > m_dwWeaponRemoveTime)
		return true;

	return false;
}

ALife::_TIME_ID	 CWeapon::TimePassedAfterIndependant()	const
{
	if(!H_Parent() && m_dwWeaponIndependencyTime != 0)
		return Level().timeServer() - m_dwWeaponIndependencyTime;
	else
		return 0;
}

bool CWeapon::can_kill	() const
{
	if (GetAmmoCurrent() || m_ammoTypes.empty())
		return				(true);

	return					(false);
}

CInventoryItem *CWeapon::can_kill	(CInventory *inventory) const
{
	if (GetAmmoElapsed() || m_ammoTypes.empty())
		return				(const_cast<CWeapon*>(this));

	TIItemSet::iterator I = inventory->m_all.begin();
	TIItemSet::iterator E = inventory->m_all.end();
	for ( ; I != E; ++I) {
		CInventoryItem	*inventory_item = smart_cast<CInventoryItem*>(*I);
		if (!inventory_item)
			continue;
		xr_vector<shared_str>::const_iterator	i = std::find(m_ammoTypes.begin(),m_ammoTypes.end(),inventory_item->cNameSect());
		if (i != m_ammoTypes.end())
			return			(inventory_item);
	}

	return					(0);
}

const CInventoryItem *CWeapon::can_kill	(const xr_vector<const CGameObject*> &items) const
{
	if (m_ammoTypes.empty())
		return				(this);

	xr_vector<const CGameObject*>::const_iterator I = items.begin();
	xr_vector<const CGameObject*>::const_iterator E = items.end();
	for ( ; I != E; ++I) {
		const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
		if (!inventory_item)
			continue;
		xr_vector<shared_str>::const_iterator	i = std::find(m_ammoTypes.begin(),m_ammoTypes.end(),inventory_item->cNameSect());
		if (i != m_ammoTypes.end())
			return			(inventory_item);
	}

	return					(0);
}

bool CWeapon::ready_to_kill	() const
{
	return					(
		!IsMisfire() && 
		((STATE == eIdle) || (STATE == eFire) || (STATE == eFire2)) && 
		GetAmmoElapsed()
	);
}



//Модификация функции для CWeapon
//отличается от оригинала в CHUDItem,тем что
//просчитывается смещения HUD в режиме приближения
void CWeapon::UpdateHudPosition	()
{
	if (m_pHUD && hud_mode)
	{
		Fmatrix							trans;

		CActor* pActor = smart_cast<CActor*>(H_Parent());
		if(pActor)
		{
			pActor->EffectorManager().affected_Matrix	(trans);
			
			if((pActor->IsZoomAimingMode() && m_fZoomRotationFactor<=1.f)
					|| (!pActor->IsZoomAimingMode() && m_fZoomRotationFactor>0.f))
			{
				Fmatrix hud_rotation;
				hud_rotation.identity();
				hud_rotation.rotateX(m_pHUD->ZoomRotateX()*m_fZoomRotationFactor);

				Fmatrix hud_rotation_y;
				hud_rotation_y.identity();
				hud_rotation_y.rotateY(m_pHUD->ZoomRotateY()*m_fZoomRotationFactor);
				hud_rotation.mulA(hud_rotation_y);

				Fvector offset = m_pHUD->ZoomOffset();
				offset.mul(m_fZoomRotationFactor);
				hud_rotation.translate_over(offset);
				trans.mulB(hud_rotation);

				if(pActor->IsZoomAimingMode())
					m_fZoomRotationFactor += Device.fTimeDelta/m_fZoomRotateTime;
				else
					m_fZoomRotationFactor -= Device.fTimeDelta/m_fZoomRotateTime;
				clamp(m_fZoomRotationFactor, 0.f, 1.f);
			}

			m_pHUD->UpdatePosition						(trans);
		}
	}
}


const Fmatrix&	CWeapon::ParticlesXFORM() const	
{
	return m_FireParticlesXForm;
//	return inherited::XFORM();
}
IRender_Sector*	CWeapon::Sector()
{
	return inherited::Sector();
}

void	CWeapon::SetAmmoElapsed	(int ammo_count)
{
	iAmmoElapsed = ammo_count;

	u32 uAmmo = u32(iAmmoElapsed);

	if (uAmmo != m_magazine.size())
	{
		if (uAmmo > m_magazine.size())
		{
			CCartridge l_cartridge; 
			l_cartridge.Load(*m_ammoTypes[m_ammoType]);
			m_fCurrentCartirdgeDisp = l_cartridge.m_kDisp;
			while (uAmmo > m_magazine.size())
			{
				m_magazine.push(l_cartridge);
			};
		}
		else
		{
			while (uAmmo < m_magazine.size())
			{
				m_magazine.pop();
			}
		};
	};
}
