#include "stdafx.h"
#include "weaponshotgun.h"
#include "WeaponHUD.h"
#include "xr_weapon_list.h"
#include "entity.h"
#include "effectorshot.h"

CWeaponShotgun::CWeaponShotgun(void) : CWeaponCustomPistol("TOZ34")
{
	m_eSoundShotBoth	= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING /*| eSoundType*/);
}

CWeaponShotgun::~CWeaponShotgun(void)
{
}
//
//void CWeaponShotgun::OnZoomIn() {
//}

void CWeaponShotgun::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	SoundDestroy		(sndShot		);
}

void CWeaponShotgun::Load	(LPCSTR section)
{
	inherited::Load		(section);

	// Звук и анимация для дуплета
	SoundCreate			(sndShotBoth,		"shoot_both"   ,m_eSoundShotBoth);
	animGet				(mhud_shot_boths,	"shoot_both");

	//
	if(pSettings->LineExists(section,"fire_point2")) vFirePoint2 = pSettings->ReadVECTOR(section,"fire_point2");
	else vFirePoint2 = vFirePoint;
}

void CWeaponShotgun::OnShot () {
	swap(m_pHUD->vFirePoint, m_pHUD->vFirePoint2);
	swap(vFirePoint, vFirePoint2);
	UpdateFP();
	inherited::OnShot();
}

void CWeaponShotgun::Fire2Start () {
	inherited::Fire2Start();
	if (IsValid())
	{
		if (!IsWorking())
		{
			if (STATE==eReload)			return;
			if (STATE==eShowing)		return;
			if (STATE==eHiding)			return;

			if (!iAmmoElapsed && iAmmoCurrent)	
			{
				CWeapon::FireStart	();
				SwitchState			(eMagEmpty);
			}
			else							
			{
				CWeapon::FireStart	();
				SwitchState			((iAmmoElapsed < iMagazineSize)?eFire:eFire2);
			}
		}
	}else{
		if (!iAmmoElapsed && !iAmmoCurrent)	
			SwitchState			(eMagEmpty);
	}
}

void CWeaponShotgun::Fire2End () {
	inherited::Fire2End();
	if (IsWorking())
	{
		CWeapon::FireEnd	();
		if (iAmmoCurrent && !iAmmoElapsed)	TryReload	();
		else								SwitchState (eIdle);
	}
}


void CWeaponShotgun::OnShotBoth()
{
	if(iAmmoElapsed < iMagazineSize) { OnShot(); return; }

	// Sound
	Sound->PlayAtPos			(sndShotBoth,H_Root(),vLastFP);

	// Camera
	if (hud_mode)	
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(new CEffectorShot(camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
	
	// Animation
	m_pHUD->animPlay			(mhud_shot_boths[Random.randI(mhud_shots.size())],FALSE);
	
	// Flames
	fFlameTime					= .1f;
	
	// Shell Drop
	OnShellDrop					();
}

void CWeaponShotgun::switch2_Fire	()
{
	if (fTime<=0)
	{
		UpdateFP					();

		// Fire
		Fvector						p1, d;
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);
		bFlame						=	TRUE;
		OnShot						();
		FireTrace					(p1,vLastFP,d);
		fTime						+= fTimeToFire;

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}
}

void CWeaponShotgun::switch2_Fire2	()
{
	if (fTime<=0)
	{
		UpdateFP					();

		// Fire
		Fvector						p1, d;
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);
		bFlame						=	TRUE;
		OnShotBoth						();
		//iAmmoElapsed	--;
		FireTrace					(p1,vLastFP,d);
		FireTrace					(p1,vLastFP,d);
		fTime						+= fTimeToFire*2.f;

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}
}

#define FLAME_TIME 0.05f
void CWeaponShotgun::OnDrawFlame	()
{
	if(STATE != eFire2) inherited::OnDrawFlame();
	else if (fFlameTime>0)	
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

		swap(m_pHUD->vFirePoint, m_pHUD->vFirePoint2);
		swap(vFirePoint, vFirePoint2);
		UpdateFP();

		// fire flash 2
		P = vLastFP;
		k=fFlameTime/FLAME_TIME;
		D; D.mul(vLastFD,::Random.randF(fFlameLength*k)/float(iFlameDiv));
		f = fFlameSize;
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


void CWeaponShotgun::OnVisible	()
{
	inherited::OnVisible	();
	if(STATE == eFire2) OnDrawFlame();
}


BOOL CWeaponShotgun::FireTrace		(const Fvector& P, const Fvector& Peff, Fvector& D)
{
	BOOL bResult = 0;
	int grape_shot = 5;
	while(grape_shot--) {
		if(!Parent) return 0;
		Collide::ray_query		RQ;

		// direct it by dispersion factor
		Fvector					dir;
		dir.random_dir			(D,(fireDispersionBase+fireDispersion*fireDispersion_Current)*GetPrecision(),Random);

		// increase dispersion
		fireDispersion_Current	+= fireDispersion_Inc;
		clamp					(fireDispersion_Current,0.f,1.f);

		// ...and trace line
		H_Parent()->setEnabled	(false);
		bResult			= Level().ObjectSpace.RayPick( P, dir, fireDistance, RQ );
		H_Parent()->setEnabled	(true);
		D						= dir;

		// ...analyze
		Fvector end_point; 
		end_point.mad		(P,D,RQ.range);
		if (bResult)
		{
			if (Local() && RQ.O) 
			{
				float power		=	float(iHitPower);
				float scale		=	1-(RQ.range/fireDistance);	clamp(scale,0.f,1.f);
				power			*=	_sqrt(scale);
				CEntity* E		=	dynamic_cast<CEntity*>(RQ.O);
				//CGameObject* GO	=	dynamic_cast<CGameObject*>(RQ.O);
				if (E) power	*=	E->HitScale(RQ.element);

				// object-space
				Fvector p_in_object_space,position_in_bone_space;
				Fmatrix m_inv;
				m_inv.invert			(RQ.O->clXFORM());
				m_inv.transform_tiny	(p_in_object_space, end_point);

				// bone-space
				CKinematics* V	=	PKinematics(RQ.O->Visual());
				Fmatrix& m_bone	=	(V->LL_GetInstance(RQ.element)).mTransform;
				Fmatrix  m_inv_bone;
				m_inv_bone.invert			(m_bone);
				m_inv_bone.transform_tiny	(position_in_bone_space, p_in_object_space);


				//  
				NET_Packet		P;
				u_EventGen		(P,GE_HIT,RQ.O->ID());
				P.w_u16			(u16(H_Parent()->ID()));
				P.w_dir			(D);
				P.w_float		(power);
				P.w_s16			((s16)RQ.element);
				P.w_vec3		(position_in_bone_space);
				u_EventSend		(P);
			}
			FireShotmark		(D,end_point,RQ);
		}
	}

	//// tracer
	//if (tracerFrame != Device.dwFrame) 
	//{
	//	tracerFrame = Device.dwFrame;
	//	Level().Tracers.Add	(Peff,end_point,tracerHeadSpeed,tracerTrailCoeff,tracerStartLength,tracerWidth);
	//}

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
