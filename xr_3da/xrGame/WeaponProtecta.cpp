#include "stdafx.h"
#include "..\effectornoise.h"
#include "..\fstaticrender.h"
#include "..\bodyinstance.h"
#include "..\xr_tokens.h"
#include "..\3DSound.h"
#include "..\PSObject.h"
#include "..\xr_trims.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponProtecta.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponProtecta::CWeaponProtecta() : CWeapon("Groza")
{
	pSounds->Create3D(sndFire,		 "weapons\\Protecta_fire");
	pSounds->Create3D(sndRicochet[0],"weapons\\ric1");
	pSounds->Create3D(sndRicochet[1],"weapons\\ric2");
	pSounds->Create3D(sndRicochet[2],"weapons\\ric3");
	pSounds->Create3D(sndRicochet[3],"weapons\\ric4");
	pSounds->Create3D(sndRicochet[4],"weapons\\ric5");

	iFlameDiv		= 0;
	fFlameLength	= 0;
	fFlameSize		= 0;
	iShotCount		= 0;

	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);
}

CWeaponProtecta::~CWeaponProtecta()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		Device.Shader.Delete(hFlames[i]);

	// sounds
	pSounds->Delete3D(sndFire);
	for (i=0; i<SND_RIC_COUNT; i++) pSounds->Delete3D(sndRicochet[i]);
}

void CWeaponProtecta::Load(CInifile* ini, const char* section){
	inherited::Load(ini, section);
	R_ASSERT(m_pHUD);
	
	vFirePoint		= ini->ReadVECTOR(section,"fire_point");
	iShotCount		= ini->ReadINT(section,"shot_count");
	
	iFlameDiv		= ini->ReadINT	(section,"flame_div");
	fFlameLength	= ini->ReadFLOAT(section,"flame_length");
	fFlameSize		= ini->ReadFLOAT(section,"flame_size");

	// flame textures
	LPCSTR S		= ini->ReadSTRING(section,"flame");
	DWORD scnt		= _GetItemCount(S);
	char name[255];
	for (DWORD i=0; i<scnt; i++)
		hFlames.push_back(Device.Shader.Create("fire_trail",_GetItem(S,i,name),false));
}

void CWeaponProtecta::FireStart()
{
	if (!IsWorking() && IsValid()){ 
		CWeapon::FireStart();
		st_target	= eFire;
		fTime		= 0;
	}
}

void CWeaponProtecta::FireEnd(){
	if (IsWorking()){
		CWeapon::FireEnd();
		m_pHUD->FireEnd();
		st_target = eIdle;
	}
}


void CWeaponProtecta::UpdateXForm(BOOL bHUDView)
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (bHUDView) {
			if (m_pHUD)	{
				Fmatrix			trans;
				trans.set(
					Device.vCameraRight,
					Device.vCameraTop,
					Device.vCameraDirection,
					Device.vCameraPosition
					);
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
			R.crossproduct	(mR.j,D);	R.normalize_safe();
			N.crossproduct	(D,R);	N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mul2		(m_pParent->clTransform);
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponProtecta::UpdateFP(BOOL bHUDView)
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		// update animation
		PKinematics V			= bHUDView?PKinematics(m_pHUD->Visual()):0;
		if (V) V->Calculate		();

		// fire point&direction
		UpdateXForm				(bHUDView);
		Fmatrix& fire_mat		= bHUDView?V->LL_GetTransform(m_pHUD->iFireBone):precalc_identity;
		Fmatrix& parent			= bHUDView?m_pHUD->Transform():clTransform;
		Fvector& fp				= bHUDView?m_pHUD->vFirePoint:vFirePoint;
		fire_mat.transform_tiny	(vLastFP,fp);
		parent.transform_tiny	(vLastFP);
		vLastFD.set				(0.f,0.f,1.f);
		parent.transform_dir	(vLastFD);
	}
}

void CWeaponProtecta::Update(float dt, BOOL bHUDView)
{
	BOOL bShot = false;

	// on state change
	if (st_target!=st_current)
	{
		if (st_target == eFire) UpdateFP(bHUDView);

		switch(st_target){
		case eIdle:
			break;
		case eFire:
			pSounds->Play3DAtPos(sndFire,vLastFP,true);
			break;
		}
		st_current=st_target;
	}

	// cycle update
	if (st_current == eFire) UpdateFP(bHUDView);
	switch (st_current)
	{
	case eIdle:
		break;
	case eFire:
		{
			fTime-=dt;
			Fvector p1, d;
			m_pParent->g_fireParams(p1,d);
			while (fTime<0)
			{
				VERIFY(m_pParent);
				fTime+=fTimeToFire;
				
				for (int i=0; i<iShotCount; i++){
					// real fire
					Collide::ray_query RQ;
					if (FireTrace( p1, d, RQ )){
						if (RQ.O){
							if (RQ.O->CLS_ID == CLSID_ENTITY)
							{
								CEntity* E = (CEntity*)RQ.O;
								E->Hit	(iHitPower,d,m_pParent);
							}
						} else {
							Fvector end; end.direct(p1,d,RQ.range);
							AddShotmark(d,end,RQ);
						}
					}
				}
				
		 		if (iAmmoElapsed==0) { m_pParent->g_fireEnd(); break; }
				m_pHUD->Shoot();
			}

			// sound fire loop
			if (sndFire.feedback) sndFire.feedback->SetPosition(vLastFP);
		}
		break;
	}

	m_pHUD->UpdateAnimation();
}

void CWeaponProtecta::Render(BOOL bHUDView)
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
	if (st_current==eFire) 
	{
		UpdateFP	(bHUDView);

		// fire flash
		Fvector P = vLastFP;
		Fvector D; D.mul(vLastFD,::Random.randF(fFlameLength)/float(iFlameDiv));
		float f = fFlameSize;
		for (int i=0; i<iFlameDiv; i++){
			f*=0.9f;
			float	S = f+f*::Random.randF	();
			float	A = ::Random.randF		(PI_MUL_2);
			::Render.add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,bHUDView);
			P.add(D);
		}
	}
}

void CWeaponProtecta::SetDefaults()
{
	CWeapon::SetDefaults();
	iAmmoElapsed = 0;
}

void CWeaponProtecta::AddShotmark(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	inherited::AddShotmark(vDir, vEnd, R);
	pSounds->Play3DAtPos(sndRicochet[Random.randI(SND_RIC_COUNT)], vEnd,false);
	
	// particles
	RAPID::tri* pTri	= pCreator->ObjectSpace.GetStaticTris()+R.element;
	Fvector N,D;
	N.mknormal(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect(vDir,N);

	CSector* S			= ::Render.getSector(pTri->sector);
// stones
	CPSObject* PS		= new CPSObject("stones",S);
	// update emitter & run
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);

// smoke
	PS					= new CPSObject("smokepuffs_1",S);
	// update emitter & run
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);
}
