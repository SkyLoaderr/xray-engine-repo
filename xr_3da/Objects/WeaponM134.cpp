#include "stdafx.h"
#include "effectorshot.h"
#include "..\render.h"
#include "..\bodyinstance.h"
#include "..\xr_tokens.h"
#include "..\3DSound.h"
#include "..\PSObject.h"
#include "..\xr_trims.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponM134.h"
#include "entity.h"
#include "xr_weapon_list.h"

void __stdcall CWeaponM134::RotateCallback_norm(CBoneInstance* B)
{
	CWeaponM134*W		= (CWeaponM134*)B->Callback_Param;
	Fmatrix&	xform	= B->mTransform;
	Fmatrix		spin;

	switch (W->bRotAxisWpn){
	case 0: spin.rotateX	(-W->fRotateAngle); break;
	case 1: spin.rotateY	(W->fRotateAngle);	break;
	case 2: spin.rotateZ	(-W->fRotateAngle); break;
	default: NODEFAULT;
	}
	xform.mulB	(spin);
}

static float AA=0.f;

void __stdcall CWeaponM134::RotateCallback_hud(CBoneInstance* B)
{
	CWeaponM134*W		= (CWeaponM134*)B->Callback_Param;
	Fmatrix&	xform	= B->mTransform;
	Fmatrix		spin;

	switch (W->bRotAxisHUD){
	case 0: spin.rotateX	(-W->fRotateAngle); break;
	case 1: spin.rotateY	(-W->fRotateAngle); break;
	case 2: spin.rotateZ	(-W->fRotateAngle); break;
	default: NODEFAULT;
	}

	xform.mulB	(spin);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponM134::CWeaponM134() : CWeapon("M134")
{
	pSounds->Create3D(sndFireStart,		"weapons\\m134_fire_start"  ,FALSE, SOUND_TYPE_WEAPON_SHOOTING_MACHINEGUN);
	pSounds->Create3D(sndFireLoop,		"weapons\\m134_fire_loop"	,FALSE, SOUND_TYPE_WEAPON_SHOOTING_MACHINEGUN);
	pSounds->Create3D(sndFireEnd,		"weapons\\m134_fire_end"	,FALSE, SOUND_TYPE_WEAPON_SHOOTING_MACHINEGUN);
	pSounds->Create3D(sndServo,			"weapons\\m134_servo"		,TRUE,  SOUND_TYPE_WEAPON_CHANGING_MACHINEGUN);
	pSounds->Create3D(sndRicochet[0],	"weapons\\generic_ric1"		,FALSE, SOUND_TYPE_WEAPON_BULLET_RICOCHET_MACHINEGUN);
	pSounds->Create3D(sndRicochet[1],	"weapons\\generic_ric2"		,FALSE, SOUND_TYPE_WEAPON_BULLET_RICOCHET_MACHINEGUN);
	pSounds->Create3D(sndRicochet[2],	"weapons\\generic_ric3"		,FALSE, SOUND_TYPE_WEAPON_BULLET_RICOCHET_MACHINEGUN);
	pSounds->Create3D(sndRicochet[3],	"weapons\\generic_ric4"		,FALSE, SOUND_TYPE_WEAPON_BULLET_RICOCHET_MACHINEGUN);
	pSounds->Create3D(sndRicochet[4],	"weapons\\generic_ric5"		,FALSE, SOUND_TYPE_WEAPON_BULLET_RICOCHET_MACHINEGUN);

	fRotateSpeed	= 0;
	fRotateAngle	= 0;
	st_current=st_target=eM134Idle;
	iWpnRotBone		= -1;
	iHUDRotBone		= -1;
	iFireBone		= -1;
	
	bRotAxisHUD		= 0;
	bRotAxisWpn		= 0;
	
	iFlameDiv		= 0;
	fFlameLength	= 0;
	fFlameSize		= 0;

	dwServoMaxFreq	= 10000;
	dwServoMinFreq	= 22050;
	
	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);
}

CWeaponM134::~CWeaponM134()
{
	FlameUNLOAD();

	// sounds
	pSounds->Delete(sndFireStart);
	pSounds->Delete(sndFireLoop);
	pSounds->Delete(sndFireEnd);
	pSounds->Delete(sndServo);
	for (int i=0; i<SND_RIC_COUNT; i++) pSounds->Delete(sndRicochet[i]);
}

void CWeaponM134::Load(CInifile* ini, const char* section)
{
	inherited::Load(ini, section);
	R_ASSERT(m_pHUD);
	
	LPCSTR rot_bone = ini->ReadSTRING	(section,"rot_bone");
	
	iWpnRotBone		= PKinematics(Visual())->LL_BoneID(rot_bone);
	iHUDRotBone		= PKinematics(m_pHUD->Visual())->LL_BoneID(rot_bone);
	
	fRotateMaxSpeed		= ini->ReadFLOAT	(section,"rot_max_speed");
	fRotateSpinupAccel	= ini->ReadFLOAT	(section,"rot_spinup_accel");
	fRotateBreakAccel	= ini->ReadFLOAT	(section,"rot_break_accel");
	
	LPCSTR fire_bone= ini->ReadSTRING	(section,"fire_bone");
	iFireBone		= PKinematics(Visual())->LL_BoneID(fire_bone);
	
	bRotAxisHUD		= ini->ReadTOKEN(section,"rot_axis_hud",axis_token);
	bRotAxisWpn		= ini->ReadTOKEN(section,"rot_axis_wpn",axis_token);
	
	iFlameDiv		= ini->ReadINT	(section,"flame_div");
	fFlameLength	= ini->ReadFLOAT(section,"flame_length");
	fFlameSize		= ini->ReadFLOAT(section,"flame_size");
	
	dwServoMaxFreq	= ini->ReadINT(section,"servo_max_freq");
	dwServoMinFreq	= ini->ReadINT(section,"servo_min_freq");
	
	PKinematics(pVisual)->PlayCycle("idle");
	PKinematics(pVisual)->LL_GetInstance(iWpnRotBone).set_callback			(RotateCallback_norm,this);
	PKinematics(m_pHUD->Visual())->LL_GetInstance(iHUDRotBone).set_callback	(RotateCallback_hud,this);

	mhud_idle		= m_pHUD->animGet("idle");	
	mhud_fire		= m_pHUD->animGet("fire");	
	mhud_spinup		= m_pHUD->animGet("spinup");	
}

void CWeaponM134::FlameLOAD()
{
	// flame textures
	LPCSTR S		= pSettings->ReadSTRING	(cName(),"flame");
	DWORD scnt		= _GetItemCount(S);
	char name[255];
	for (DWORD i=0; i<scnt; i++)
		hFlames.push_back(Device.Shader.Create("effects\\flame",_GetItem(S,i,name),false));
}
void CWeaponM134::FlameUNLOAD()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		Device.Shader.Delete(hFlames[i]);
	hFlames.clear();
}

void CWeaponM134::OnDeviceCreate()
{
	REQ_CREATE	();
	inherited::OnDeviceCreate	();
	FlameLOAD	();
}

void CWeaponM134::OnDeviceDestroy()
{
	inherited::OnDeviceDestroy	();
	FlameUNLOAD	();
}

void CWeaponM134::FireStart()
{
	if (!IsWorking() && IsValid())
	{
		inherited::FireStart();
		m_pHUD->animPlay		(mhud_spinup);
		st_target = eM134Spinup;
	}
}

void CWeaponM134::FireEnd(){
	if (IsWorking()){
		inherited::FireEnd	();
		m_pHUD->animPlay		(mhud_idle);
		st_target = eM134Brake;
	}
}

void CWeaponM134::UpdateXForm(BOOL bHUDView)
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (bHUDView) {
			if (m_pHUD)	{
				Fmatrix			trans;
				Level().Cameras.affected_Matrix(trans);
				m_pHUD->UpdatePosition(trans);
			}
		} else {
			Fmatrix mRes;
			CKinematics* V	= PKinematics		(H_Parent()->Visual());
			V->Calculate	();
			Fmatrix& mL		= V->LL_GetTransform(m_pContainer->m_iACTboneL);
			Fmatrix& mR		= V->LL_GetTransform(m_pContainer->m_iACTboneR);
			
			Fvector			R,D,N;
			D.sub			(mL.c,mR.c);	D.normalize_safe();
			R.crossproduct	(mR.j,D);		R.normalize_safe();
			N.crossproduct	(D,R);			N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mulA		(H_Parent()->clXFORM());
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponM134::UpdateFP	(BOOL bHUDView)
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		// update animation
		CKinematics* V	= bHUDView?PKinematics(m_pHUD->Visual()):PKinematics(Visual());
		V->Calculate	();

		// fire point&direction
		UpdateXForm				(bHUDView);
		Fmatrix& fire_mat		= V->LL_GetTransform(bHUDView?m_pHUD->iFireBone:iFireBone);
		Fmatrix& parent			= bHUDView?m_pHUD->Transform():svTransform;
		Fvector& fp				= bHUDView?m_pHUD->vFirePoint:vFirePoint;
		Fvector& sp				= bHUDView?m_pHUD->vShellPoint:vShellPoint;
		fire_mat.transform_tiny	(vLastFP,fp);
		parent.transform_tiny	(vLastFP);
		fire_mat.transform_tiny	(vLastSP,sp);
		parent.transform_tiny	(vLastSP);
		vLastFD.set				(0.f,0.f,1.f);
		parent.transform_dir	(vLastFD);
	}
}

void CWeaponM134::Update	(float dt, BOOL bHUDView)
{
	BOOL bShot = false;

	inherited::Update(dt,bHUDView);

	// on state change
	if (st_target!=st_current)
	{
		if (st_target != eM134Idle) UpdateFP(bHUDView);

		switch(st_target){
		case eM134Spinup:
			bPending		= TRUE;
			fTime			= 0;
			// play start fire sound
			pSounds->Play3DAtPos(sndFireStart,	this,vLastFP,false);
			pSounds->Play3DAtPos(sndServo,		this,vLastFP,true);
			break;
		case eM134Brake:
			// sound fire loop
			if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();

			// play end fire sound
			if (st_current==eM134Fire)
				pSounds->Play3DAtPos	(sndFireEnd,this,vLastFP,false);
			pSounds->Play3DAtPos	(sndServo,this,vLastFP,true);
			break;
		case eM134Idle:
			bPending		= FALSE;
			if (st_current==eM134Brake) {
				if (sndServo.feedback) sndServo.feedback->Stop();
			}
			break;
		case eM134Fire:
			if (st_current==eM134Spinup)
			{
				m_pHUD->animPlay		(mhud_fire);
				pSounds->Play3DAtPos	(sndFireLoop,this,vLastFP,true);
				if (sndServo.feedback) sndServo.feedback->Stop();
			}
			break;
		}
		st_current=st_target;
	}

	// cycle update
	if (st_current != eM134Idle) UpdateFP(bHUDView);
	switch (st_current)
	{
	case eM134Idle:
		break;
	case eM134Spinup:
		fRotateSpeed += fRotateSpinupAccel*dt;
		if (sndServo.feedback){
			sndServo.feedback->SetPosition(vLastFP);
			float k = sqrtf(fRotateSpeed/fRotateMaxSpeed);
			DWORD freq=iFloor((dwServoMaxFreq-dwServoMinFreq)*k+dwServoMinFreq);
			sndServo.feedback->SetFrequency(freq);
		}
		if (fRotateSpeed>=fRotateMaxSpeed){
			fRotateSpeed = fRotateMaxSpeed;
			st_target = eM134Fire;
		}
		break;
	case eM134Brake:  
		fRotateSpeed -= fRotateBreakAccel*dt;
		if (sndServo.feedback){
			sndServo.feedback->SetPosition(vLastFP);
			float k = sqrtf(fRotateSpeed/fRotateMaxSpeed);
			DWORD freq=iFloor((dwServoMaxFreq-dwServoMinFreq)*k+dwServoMinFreq);
			sndServo.feedback->SetFrequency(freq);
		}
		if (fRotateSpeed<=0){
			st_target		= eM134Idle;
			fRotateSpeed	= 0;
		}
		break;
	case eM134Fire:
		{
			fTime-=dt;
			Fvector p1, d;
			dynamic_cast<CEntity*>(H_Parent())->g_fireParams(p1,d);

			while ((fTime<0)  && iAmmoElapsed)
			{
				fTime			+=fTimeToFire;
				
				if (bHUDView)	{
					CEffectorShot*	S		= (CEffectorShot*)Level().Cameras.GetEffector(cefShot); 
					if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(new CEffectorShot(camMaxAngle,camRelaxSpeed));
					R_ASSERT(S); 
					S->Shot					(camDispersion); 
				}

				UpdateFP		(bHUDView);
				FireTrace		(p1,vLastFP,d);
			}

			// sound fire loop
			if (sndFireLoop.feedback) sndFireLoop.feedback->SetPosition(vLastFP);
			if (0==iAmmoElapsed) st_target = eM134Brake;
		}
		break;
	}
	fRotateAngle	+= fRotateSpeed*dt;

#ifdef DEBUG
	pApp->pFont->Out(0,0,"%d",iAmmoElapsed);
	pApp->pFont->Out(0,0.05f,"%.2f",fRotateSpeed);
	pApp->pFont->Out(0,0.1f,"%.2f",fRotateAngle);
#endif
}

void CWeaponM134::Render	(BOOL bHUDView)
{
	inherited::Render		(bHUDView);
	UpdateXForm	(bHUDView);
	if (bHUDView && m_pHUD)
	{ 
		// HUD render
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else
	{
		// Actor render
		::Render->set_Transform		(&svTransform);
		::Render->add_Visual		(Visual());
	}
	if (st_current==eM134Fire) 
	{
		UpdateFP	(bHUDView);
		OnDrawFlame	(bHUDView);
	}
}

void CWeaponM134::SetDefaults	()
{
	CWeapon::SetDefaults();
	iAmmoElapsed = 0;
}

void CWeaponM134::Hide			()
{
	inherited::Hide				();
	signal_HideComplete			();
}
void CWeaponM134::Show			()
{
	inherited::Show				();
}

void CWeaponM134::FireShotmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	inherited::FireShotmark		(vDir, vEnd, R);
	pSounds->Play3DAtPos		(sndRicochet[Random.randI(SND_RIC_COUNT)], H_Root(), vEnd,false);

	// particles
	Fvector N,D;
	CDB::TRI* pTri		= pCreator->ObjectSpace.GetStaticTris()+R.element;
	N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect			(vDir,N);

	CSector* S			= ::Render->getSector(pTri->sector);

	// stones or sparks
	LPCSTR ps_gibs		= (Random.randI(5)==0)?"sparks_1":"stones";
	CPSObject* PS		= new CPSObject(ps_gibs,S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);

	// smoke
	PS					= new CPSObject("smokepuffs_1",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);
}
