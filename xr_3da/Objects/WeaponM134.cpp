#include "stdafx.h"
#include "effectorshot.h"
#include "..\fstaticrender.h"
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
	case 1: spin.rotateY	(-W->fRotateAngle); break;
	case 2: spin.rotateZ	(-W->fRotateAngle); break;
	default: NODEFAULT;
	}
	xform.mul	(spin);
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

	xform.mul	(spin);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponM134::CWeaponM134() : CWeapon("M134")
{
	pSounds->Create3D(sndFireStart,	"weapons\\m134_fire_start");
	pSounds->Create3D(sndFireLoop,	"weapons\\m134_fire_loop");
	pSounds->Create3D(sndFireEnd,	"weapons\\m134_fire_end");
	pSounds->Create3D(sndServo,		"weapons\\m134_servo",TRUE);
	pSounds->Create3D(sndRicochet[0],"weapons\\ric1");
	pSounds->Create3D(sndRicochet[1],"weapons\\ric2");
	pSounds->Create3D(sndRicochet[2],"weapons\\ric3");
	pSounds->Create3D(sndRicochet[3],"weapons\\ric4");
	pSounds->Create3D(sndRicochet[4],"weapons\\ric5");

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
	for (DWORD i=0; i<hFlames.size(); i++)
		Device.Shader.Delete(hFlames[i]);

	// sounds
	pSounds->Delete3D(sndFireStart);
	pSounds->Delete3D(sndFireLoop);
	pSounds->Delete3D(sndFireEnd);
	pSounds->Delete3D(sndServo);
	for (i=0; i<SND_RIC_COUNT; i++) pSounds->Delete3D(sndRicochet[i]);
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
	
	vFirePoint		= ini->ReadVECTOR(section,"fire_point");
	
	bRotAxisHUD		= ini->ReadTOKEN(section,"rot_axis_hud",axis_token);
	bRotAxisWpn		= ini->ReadTOKEN(section,"rot_axis_wpn",axis_token);
	
	iFlameDiv		= ini->ReadINT	(section,"flame_div");
	fFlameLength	= ini->ReadFLOAT(section,"flame_length");
	fFlameSize		= ini->ReadFLOAT(section,"flame_size");

	// flame textures
	LPCSTR S		= ini->ReadSTRING(section,"flame");
	DWORD scnt		= _GetItemCount(S);
	char name[255];
	for (DWORD i=0; i<scnt; i++)
		hFlames.push_back(Device.Shader.Create("fire_trail",_GetItem(S,i,name),false));

	dwServoMaxFreq	= ini->ReadINT(section,"servo_max_freq");
	dwServoMinFreq	= ini->ReadINT(section,"servo_min_freq");

	PKinematics(pVisual)->PlayCycle("idle");
	PKinematics(pVisual)->LL_GetInstance(iWpnRotBone).set_callback			(RotateCallback_norm,this);

	PKinematics(m_pHUD->Visual())->PlayCycle("idle");
	PKinematics(m_pHUD->Visual())->LL_GetInstance(iHUDRotBone).set_callback	(RotateCallback_hud,this);
}

void CWeaponM134::FireStart()
{
	if (!IsWorking() && IsValid()){ 
		CWeapon::FireStart();
		m_pHUD->FireSpinup();
		st_target = eM134Spinup;
	}
}

void CWeaponM134::FireEnd(){
	if (IsWorking()){
		CWeapon::FireEnd();
		m_pHUD->FireEnd();
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
				Level().Cameras.unaffected_Matrix(trans);
				m_pHUD->UpdatePosition(trans);
			}
		} else {
			Fmatrix mRes;
			PKinematics V	= PKinematics(m_pParent->Visual());
			V->Calculate	();
			Fmatrix& mL		= V->LL_GetTransform(m_pContainer->m_iACTboneL);
			Fmatrix& mR		= V->LL_GetTransform(m_pContainer->m_iACTboneR);
			
			Fvector			R,D,N;
			D.sub			(mL.c,mR.c);	D.normalize_safe();
			R.crossproduct	(mR.j,D);		R.normalize_safe();
			N.crossproduct	(D,R);			N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mul2		(m_pParent->clTransform);
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponM134::UpdateFP(BOOL bHUDView)
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		// update animation
		PKinematics V	= bHUDView?PKinematics(m_pHUD->Visual()):PKinematics(Visual());
		V->Calculate	();

		// fire point&direction
		UpdateXForm				(bHUDView);
		Fmatrix& fire_mat		= V->LL_GetTransform(bHUDView?m_pHUD->iFireBone:iFireBone);
		Fmatrix& parent			= bHUDView?m_pHUD->Transform():svTransform;
		Fvector& fp				= bHUDView?m_pHUD->vFirePoint:vFirePoint;
		fire_mat.transform_tiny	(vLastFP,fp);
		parent.transform_tiny	(vLastFP);
		vLastFD.set				(0.f,0.f,1.f);
		parent.transform_dir	(vLastFD);
	}
}

void CWeaponM134::Update(float dt, BOOL bHUDView)
{
	BOOL bShot = false;

	// on state change
	if (st_target!=st_current)
	{
		if (st_target != eM134Idle) UpdateFP(bHUDView);

		switch(st_target){
		case eM134Spinup:
			bPending		= TRUE;
			fTime			= 0;
			// play start fire sound
			pSounds->Play3DAtPos(sndFireStart,	vLastFP,false);
			pSounds->Play3DAtPos(sndServo,		vLastFP,true);
			break;
		case eM134Brake:
			if (bHUDView)	Level().Cameras.RemoveEffector(cefShot);
			// sound fire loop
			if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();

			// play end fire sound
			if (st_current==eM134Fire)
				pSounds->Play3DAtPos(sndFireEnd,vLastFP,false);
			pSounds->Play3DAtPos(sndServo,vLastFP,true);
			break;
		case eM134Idle:
			bPending		= FALSE;
			if (st_current==eM134Brake) {
				if (sndServo.feedback) sndServo.feedback->Stop();
			}
			break;
		case eM134Fire:
			if (st_current==eM134Spinup){
				m_pHUD->FireCycleStart();
				pSounds->Play3DAtPos(sndFireLoop,vLastFP,true);
				if (sndServo.feedback) sndServo.feedback->Stop();
			}
			if (bHUDView)	Level().Cameras.AddEffector(new CEffectorShot(camRelax,camDispersion));
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
			m_pParent->g_fireParams(p1,d);

			while (fTime<0)
			{
				fTime			+=fTimeToFire;
				
				if (bHUDView)	{
					CEffectorShot*	S = (CEffectorShot*)Level().Cameras.GetEffector(cefShot);
					if (S)			S->Shot();
				}

				UpdateFP		(bHUDView);
				FireTrace		(p1,vLastFP,d);
				iAmmoElapsed	--;
		 		if (iAmmoElapsed==0) { m_pParent->g_fireEnd(); break; }
			}

			// sound fire loop
			if (sndFireLoop.feedback) sndFireLoop.feedback->SetPosition(vLastFP);
		}
		break;
	}
	fRotateAngle	+= fRotateSpeed*dt;

	m_pHUD->UpdateAnimation();
	
#ifdef DEBUG
	pApp->pFont->Out(0,0,"%d",iAmmoElapsed);
	pApp->pFont->Out(0,0.05f,"%.2f",fRotateSpeed);
	pApp->pFont->Out(0,0.1f,"%.2f",fRotateAngle);
#endif
}

void CWeaponM134::Render(BOOL bHUDView)
{
	UpdateXForm	(bHUDView);
	if (bHUDView)
	{ 
		// Interpolate lighting
		float l_f	= Device.fTimeDelta*fLightSmoothFactor;
		float l_i	= 1.f-l_f;
		float&	LL	= m_pParent->AI_Lighting;
		NodeCompressed*	NODE = m_pParent->AI_Node;
		float	CL	= NODE?float(NODE->light):255;
		LL			= l_i*LL + l_f*CL; 

		// HUD render
		if (m_pHUD) Level().HUD()->RenderModel(m_pHUD->Visual(),m_pHUD->Transform(),iFloor(LL));
	}
	else
	{
		// Actor render
		::Render.set_Transform		(&svTransform);
		::Render.set_LightLevel		(iFloor(m_pParent->AI_Lighting));
		::Render.add_leafs_Dynamic	(Visual());
	}
	if (st_current==eM134Fire) 
	{
		UpdateFP	(bHUDView);

		// fire flash
		Fvector P = vLastFP;
		Fvector D; D.mul(vLastFD,::Random.randF(fFlameLength)/float(iFlameDiv));
		float f = fFlameSize;
		for (int i=0; i<iFlameDiv; i++)
		{
			f*=0.9f;
			float	S = f+f*::Random.randF	();
			float	A = ::Random.randF		(PI_MUL_2);
			::Render.add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,bHUDView);
			P.add(D);
		}
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
	pSounds->Play3DAtPos		(sndRicochet[Random.randI(SND_RIC_COUNT)], vEnd,false);

	// particles
	Fvector N,D;
	RAPID::tri* pTri	= pCreator->ObjectSpace.GetStaticTris()+R.element;
	N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect			(vDir,N);

	CSector* S			= ::Render.getSector(pTri->sector);

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
