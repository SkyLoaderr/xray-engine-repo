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
#include "WeaponGroza.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponGroza::CWeaponGroza() : CWeapon("GROZA")
{
	pSounds->Create3D(sndFireLoop,	 "weapons\\Groza_fire");
	pSounds->Create3D(sndRicochet[0],"weapons\\ric1");
	pSounds->Create3D(sndRicochet[1],"weapons\\ric2");
	pSounds->Create3D(sndRicochet[2],"weapons\\ric3");
	pSounds->Create3D(sndRicochet[3],"weapons\\ric4");
	pSounds->Create3D(sndRicochet[4],"weapons\\ric5");

	iFlameDiv		= 0;
	fFlameLength	= 0;
	fFlameSize		= 0;

	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);
	
	fTime			= 0;
}

CWeaponGroza::~CWeaponGroza()
{
	for (DWORD i=0; i<hFlames.size(); i++)
		Device.Shader.Delete(hFlames[i]);

	// sounds
	pSounds->Delete3D(sndFireLoop);
	for (i=0; i<SND_RIC_COUNT; i++) pSounds->Delete3D(sndRicochet[i]);
}

void CWeaponGroza::Load(CInifile* ini, const char* section){
	inherited::Load(ini, section);
	R_ASSERT		(m_pHUD);
	
	vFirePoint		= ini->ReadVECTOR(section,"fire_point");
	
	iFlameDiv		= ini->ReadINT	(section,"flame_div");
	fFlameLength	= ini->ReadFLOAT(section,"flame_length");
	fFlameSize		= ini->ReadFLOAT(section,"flame_size");

	// flame textures
	LPCSTR S		= ini->ReadSTRING(section,"flame");
	DWORD scnt		= _GetItemCount(S);
	char name[255];
	for (DWORD i=0; i<scnt; i++)
		hFlames.push_back(Device.Shader.Create("fire_trail",_GetItem(S,i,name),false));
	bFlame			= FALSE;
}

void CWeaponGroza::FireStart()
{
	if (!IsWorking() && IsValid()){ 
		CWeapon::FireStart();
		st_target	= eFire;
	}
}

void CWeaponGroza::FireEnd	()
{
	if (IsWorking())
	{
		CWeapon::FireEnd	();
		m_pHUD->FireEnd		();
		st_target			= eIdle;
	}
}

void CWeaponGroza::UpdateXForm(BOOL bHUDView)
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (bHUDView) 
		{
			if (m_pHUD)	
			{
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
			mRes.mul2_43	(m_pParent->clTransform);
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponGroza::UpdateFP(BOOL bHUDView)
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;
		
		UpdateXForm		(bHUDView);

		if (bHUDView)	
		{
			// 1st person view - skeletoned
			PKinematics V			= PKinematics(m_pHUD->Visual());
			V->Calculate			();
			
			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(m_pHUD->iFireBone);
			Fmatrix& parent			= m_pHUD->Transform();
			Fvector& fp				= m_pHUD->vFirePoint;
			fire_mat.transform_tiny	(vLastFP,fp);
			parent.transform_tiny	(vLastFP);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		} else {
			// 3rd person
			Fmatrix& parent			= svTransform;
			Fvector& fp				= vFirePoint;
			parent.transform_tiny	(vLastFP,fp);
			vLastFD.set				(0.f,0.f,1.f);
			parent.transform_dir	(vLastFD);
		}
	}
}

void CWeaponGroza::Update(float dt, BOOL bHUDView)
{
	BOOL bShot = false;

	// on state change
	if (st_target!=st_current)
	{
		if (st_target == eFire) UpdateFP(bHUDView);

		switch(st_target)
		{
		case eIdle:
			if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
			if (bHUDView)	Level().Cameras.RemoveEffector	(cefShot);
			bFlame			= FALSE;
			break;
		case eFire:
			if (sndFireLoop.feedback) sndFireLoop.feedback->Stop();
			pSounds->Play3DAtPos(sndFireLoop,vLastFP,true);
			break;
		}
		st_current=st_target;
	}

	// cycle update
	if (st_current == eFire) UpdateFP(bHUDView);
	switch (st_current)
	{
	case eIdle:
		fTime	-= dt;
		if (fTime<0)	fTime = 0;
		break;
	case eFire:
		{
			VERIFY		(m_pParent);
			fTime		-=dt;
			Fvector		p1, d;
			m_pParent->g_fireParams(p1,d);
			
			while (fTime<0)
			{
				bFlame			= TRUE;
				fTime			+=fTimeToFire;

				if (bHUDView)	{
					CEffectorShot*	S = (CEffectorShot*)Level().Cameras.GetEffector(cefShot);
					if (S)			S->Shot();
				}

				UpdateFP		(bHUDView);
				FireTrace		(p1,vLastFP,d);
				iAmmoElapsed	--;
		 		if (iAmmoElapsed==0) { m_pParent->g_fireEnd(); break; }
				m_pHUD->Shoot	();
			}

			// sound fire loop
			if (sndFireLoop.feedback) sndFireLoop.feedback->SetPosition(vLastFP);
		}
		break;
	}

	m_pHUD->UpdateAnimation();
}

void CWeaponGroza::Render(BOOL bHUDView)
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
	if ((st_current==eFire) && bFlame) 
	{
		if (bHUDView &&	(0==Level().Cameras.GetEffector(cefShot)))	Level().Cameras.AddEffector(new CEffectorShot(camRelax,camDispersion));

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

void CWeaponGroza::SetDefaults	()
{
	CWeapon::SetDefaults		();
	iAmmoElapsed				= 0;
}

void CWeaponGroza::Hide			()
{
	inherited::Hide				();
	signal_HideComplete			();
}
void CWeaponGroza::Show			()
{
	inherited::Show				();
}

void CWeaponGroza::FireShotmark(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	inherited::FireShotmark	(vDir, vEnd, R);
	pSounds->Play3DAtPos	(sndRicochet[Random.randI(SND_RIC_COUNT)], vEnd,false);
	
	// particles
	Fvector N,D;
	RAPID::tri* pTri	= pCreator->ObjectSpace.GetStaticTris()+R.element;
	N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect			(vDir,N);

	CSector* S			= ::Render.getSector(pTri->sector);

	// stones
	CPSObject* PS		= new CPSObject("stones",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);

	// smoke
	PS					= new CPSObject("smokepuffs_1",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);
}
