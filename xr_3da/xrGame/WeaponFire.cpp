// WeaponFire.cpp: implementation of the CWeapon class.
// function responsible for firing with CWeapon
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "effectorshot.h"
#include "Weapon.h"
#include "WeaponHUD.h"

#include "ParticlesObject.h"

#include "HUDManager.h"
#include "entity.h"
#include "actor.h"


#define FLAME_TIME 0.05f
#define HIT_POWER_EPSILON 0.05f



void CWeapon::FireShotmark	(const Fvector& /**vDir/**/, const Fvector &vEnd, Collide::rq_result& R, u16 target_material) 
{
	SGameMtlPair* mtl_pair	= GMLib.GetMaterialPair(CWeapon::bullet_material_id, target_material);

	
	Fvector particle_dir;
	if (R.O) 
	{
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
			particle_dir = m_vCurrentShootDir;
			particle_dir.invert();
		}
	} 
	else 
	{
        Fvector N;
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
		particle_dir = N;

		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty())?
					 NULL:
					 &mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;
		
		if (pWallmarkShader)
		{
			//добавить отметку на материале
			::Render->add_Wallmark	(
				*pWallmarkShader,
				vEnd,
				fWallmarkSize,
				pTri,
				pVerts);
		}
	}		
	
	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty())?
						 NULL:
						 &mtl_pair->CollideSounds[::Random.randI(0,mtl_pair->CollideSounds.size())];
	//проиграть звук
	if(pSound)
	{
		pSound->play_at_pos_unlimited(this, vEnd, false);
	}
    
	LPCSTR ps_name = (!mtl_pair || mtl_pair->CollideParticles.empty())?
						NULL:
						*mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];
	if(ps_name)
	{
		//отыграть партиклы попадания в материал
		CParticlesObject* pStaticPG;
		pStaticPG = xr_new<CParticlesObject>(ps_name,Sector());


		Fmatrix pos;
		Fvector zero_vel = {0.f,0.f,0.f};
		pos.k.normalize(particle_dir);
		Fvector::generate_orthonormal_basis(pos.k, pos.i, pos.j);
		pos.c.set(vEnd);

		pStaticPG->UpdateParent(pos,zero_vel);
		pStaticPG->Play();
	}
}


//callback функция 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-продолжить трассировку / FALSE-закончить трассировку
BOOL __stdcall firetrace_callback(Collide::rq_result& result, LPVOID params)
{
	
	CWeapon* pThisWeapon  = (CWeapon*)params;
	//вычислить точку попадания
	pThisWeapon->m_vEndPoint.mad(pThisWeapon->m_vCurrentShootPos,
								 pThisWeapon->m_vCurrentShootDir,result.range);

	u16 hit_material_id = GAMEMTL_NONE;

	//динамический объект
	if(result.O)
	{
		//получить косточку и ее материал
		CKinematics* V = 0;
		if (0!=(V=PKinematics(result.O->Visual())))
		{
			CBoneData& B = V->LL_GetData((u16)result.element);
			hit_material_id = B.game_mtl_idx;
			pThisWeapon->DynamicObjectHit(result, hit_material_id);
		}
#pragma todo("Dima to Yura : why this branch is not processed?? Вылетает на уровне fog_dima если стрельнуть в бетонный блок")
	}
	//статический объект
	else
	{
		//получить треугольник и узнать его материал
		CDB::TRI* T		= Level().ObjectSpace.GetStaticTris()+result.element;
		hit_material_id = T->material;
		pThisWeapon->StaticObjectHit(result, hit_material_id);
	}

	SGameMtl* mtl = GMLib.GetMaterialByIdx(hit_material_id);

	pThisWeapon->m_fCurrentHitPower*= mtl->fShootFactor;

	if(pThisWeapon->m_fCurrentHitPower>HIT_POWER_EPSILON)
		return TRUE;
	else
		return FALSE;
}

void CWeapon::DynamicObjectHit (Collide::rq_result& R, u16 target_material)
{
	//только для динамических объектов
	R_ASSERT(R.O);


    //получить силу хита выстрела с учетом патрона
	float power = float(m_fCurrentHitPower) * m_pCurrentCartridge->m_kHit;
				
	//уменьшение силы с расстоянием
	float scale = 1-(R.range/(fireDistance*m_pCurrentCartridge->m_kDist));
	clamp(scale,0.f,1.f);
	scale = _sqrt(scale);
	power *= scale;

	//сила хита физического импульса
	float impulse = m_fCurrentHitImpulse * m_pCurrentCartridge->m_kImpulse * scale;

	CEntity* E = dynamic_cast<CEntity*>(R.O);
	//учитываем попадание в разные части 
	if(E) power *= E->HitScale(R.element);

	// object-space
	//вычислить координаты попадания
	Fvector p_in_object_space,position_in_bone_space;
	Fmatrix m_inv;
	m_inv.invert(R.O->XFORM());
	m_inv.transform_tiny(p_in_object_space, m_vEndPoint);

	// bone-space
	CKinematics* V = PKinematics(R.O->Visual());
		
	if(V)
	{
		Fmatrix& m_bone = (V->LL_GetBoneInstance(u16(R.element))).mTransform;
		Fmatrix  m_inv_bone;
		m_inv_bone.invert(m_bone);
		m_inv_bone.transform_tiny(position_in_bone_space, p_in_object_space);
	}
	else
	{
		position_in_bone_space.set(p_in_object_space);
	}

	//отправить хит пораженному объекту
	NET_Packet		P;
	u_EventGen		(P,GE_HIT,R.O->ID());
	P.w_u16			(u16(H_Parent()->ID()));
	P.w_dir			(m_vCurrentShootDir);
	P.w_float		(power);
	P.w_s16			((s16)R.element);
	P.w_vec3		(position_in_bone_space);
	P.w_float		(impulse);
	P.w_u16			(eHitTypeWound);
	u_EventSend		(P);

	//визуальное обозначение попадание на объекте
	FireShotmark(m_vCurrentShootDir, m_vEndPoint, R, target_material);

	//уменьшить хит и импульс передать его дальше 
	m_fCurrentHitPower *= scale;
	m_fCurrentHitImpulse *= scale;
}

void CWeapon::StaticObjectHit(Collide::rq_result& R, u16 target_material)
{
	FireShotmark(m_vCurrentShootDir, m_vEndPoint, R, target_material);
}



BOOL CWeapon::FireTrace		(const Fvector& P, const Fvector& Peff, Fvector& D)
{
	R_ASSERT(m_magazine.size());

	CCartridge &l_cartridge = m_magazine.top();

	// direct it by dispersion factor
	Fvector dir;
	//dir.random_dir(D,(fireDispersionBase+fireDispersion*fireDispersion_Current)*GetPrecision(),Random);
	//dir.random_dir(D,(fireDispersion*fireDispersion_Current)*
	//				  GetPrecision()*
	//				  GetConditionDispersionFactor(),Random);
	float fire_disp = fireDispersion*fireDispersion_Current*
							GetPrecision()*GetConditionDispersionFactor();

	// increase dispersion
	fireDispersion_Current += fireDispersion_Inc;
	clamp(fireDispersion_Current,0.f,1.f);

	//повысить изношенность оружия
	ChangeCondition(-conditionDecreasePerShot);


	BOOL bResult = false;


	for(int i = 0; i < l_cartridge.m_buckShot; ++i) 
	{
		//dir1.random_dir(dir, fireDispersionBase * l_cartridge.m_kDisp, Random);
		//D = dir1;
		//разброс учитывая коеффициент дисперсии патрона
		float fire_disp_with_ammo = fire_disp+fireDispersionBase * l_cartridge.m_kDisp;

/*		dir = D;
		dir.x += RandNormal(0.f, fire_disp_with_ammo);
		dir.y += RandNormal(0.f, fire_disp_with_ammo);
		dir.z += RandNormal(0.f, fire_disp_with_ammo);*/

		dir.random_dir(D, fire_disp_with_ammo, Random);
		D = dir;

		//инициализипровать текущие параметры выстрела перед запуском RayPick
		m_fCurrentHitPower	= float(iHitPower);
		m_fCurrentHitImpulse = fHitImpulse;
		m_pCurrentCartridge = &l_cartridge;
		m_vCurrentShootDir = D;
		m_vCurrentShootPos = P;

		// ...and trace line
		m_vEndPoint.mad(m_vCurrentShootPos,	m_vCurrentShootDir,fireDistance*l_cartridge.m_kDist);

		Collide::ray_defs RD(P, D, fireDistance*l_cartridge.m_kDist,0,Collide::rqtBoth);
		H_Parent()->setEnabled(false);
		bResult |= Level().ObjectSpace.RayQuery( RD, firetrace_callback, this );
		H_Parent()->setEnabled(true);

		// tracer
		if(l_cartridge.m_tracer && tracerFrame != Device.dwFrame) 
		{
			tracerFrame = Device.dwFrame;
			Level().Tracers.Add	(Peff,m_vEndPoint,tracerHeadSpeed,
								 tracerTrailCoeff,tracerStartLength,tracerWidth);
		}
	}

	// light
	if(m_shotLight) Light_Start();
	
	// Ammo
	if(Local()) 
	{
		m_abrasion		= _max(0.f, m_abrasion - l_cartridge.m_impair);
		m_magazine.pop	();
		--iAmmoElapsed;
		if(iAmmoElapsed==0) 
			OnMagazineEmpty();

		//проверить не произошла ли осечка
		CheckForMisfire();
	}

	
	return				bResult;
}

//нарисовать вспышку пламени из оружия
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
		for (int i=0; i<iFlameDiv; ++i)
		{
			f		*= 0.9f;
			float	S = f+f*::Random.randF	();
			float	A = ::Random.randF		(PI_MUL_2);
			::Render->add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,hud_mode);
			P.add(D);
		}
		*/

		
/*		CParticlesObject* pStaticPG;
		pStaticPG = xr_new<CParticlesObject>(m_sFlameParitcles,Sector(),false);

*/
		/*if(m_pFlameParticles && m_pFlameParticles->PSI_alive())
		{
			UpdateFlameParticles();
		}*/
		
		


		fFlameTime -= Device.fTimeDelta;


	}
}

void CWeapon::FireStart	()
{
	bWorking=true;	
}
void CWeapon::FireEnd	()				
{ 
	bWorking=false;	
	StopFlameParticles	();
}
void CWeapon::Fire2Start()				
{ 
	bWorking2=true;	
}
void CWeapon::Fire2End	()
{ 
	bWorking2=false;
	StopFlameParticles	();
}


void CWeapon::StartFlameParticles	()
{
	//StartParticles(m_pFlameParticles, m_sFlameParitcles, vLastFP);
	if(!m_sFlameParitcles) return;

	if(m_pFlameParticles != NULL) 
	{
		UpdateFlameParticles();
		return;
	}

	m_pFlameParticles = xr_new<CParticlesObject>(m_sFlameParitcles,Sector(),false);

	UpdateFlameParticles();
	m_pFlameParticles->Play();
}
void CWeapon::StopFlameParticles	()
{
//	StopParticles(m_pFlameParticles);
	if(!m_sFlameParitcles) return;

	if(m_pFlameParticles == NULL) return;

	m_pFlameParticles->Stop();
	m_pFlameParticles->PSI_destroy();
	m_pFlameParticles = NULL;
}

void CWeapon::UpdateFlameParticles	()
{
//	UpdateParticles	(m_pFlameParticles, vLastFP);
	if(!m_sFlameParitcles) return;

	Fmatrix pos; 
	pos.set(XFORM()); 
	pos.c.set(vLastFP);
	m_pFlameParticles->SetXFORM(pos);

	
	if(!m_pFlameParticles->IsLooped() && !m_pFlameParticles->PSI_alive())
	{
		m_pFlameParticles->Stop();
		m_pFlameParticles->PSI_destroy();
		m_pFlameParticles = NULL;
	}
}

/*	Fvector vel; 
	vel.sub(Position(),ps_Element(0).vPosition); 
	vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);
	m_pFlameParticles->UpdateParent(pos, vel); 
*/

//партиклы дыма
void CWeapon::StartSmokeParticles	()
{
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, m_sSmokeParitcles, vLastFP, zero_vel, true);
	/*if(!m_sSmokeParitcles) return;

	CParticlesObject* pSmokeParticles = xr_new<CParticlesObject>(m_sSmokeParitcles,Sector());

	Fvector vel; 
	vel.sub(Position(),ps_Element(0).vPosition); 
	vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);

	Fmatrix pos; 
	pos.set(XFORM()); 
	pos.c.set(vLastFP);

	pSmokeParticles->UpdateParent(pos, vel); 
	pSmokeParticles->Play();*/
}



void CWeapon::StartParticles (CParticlesObject*& pParticles, LPCSTR particles_name, 
							  const Fvector& pos, const  Fvector& vel, bool auto_remove_flag)
{
	if(!particles_name) return;

	if(pParticles != NULL) 
	{
		UpdateParticles(pParticles, pos, vel);
		return;
	}

	pParticles = xr_new<CParticlesObject>(particles_name,Sector(),auto_remove_flag);

	UpdateParticles(pParticles, pos, vel);
	pParticles->Play();

}
void CWeapon::StopParticles (CParticlesObject*&	pParticles)
{
	if(pParticles == NULL) return;

	pParticles->Stop();
	pParticles->PSI_destroy();
	pParticles = NULL;

}
void CWeapon::UpdateParticles (CParticlesObject*& pParticles, 
							   const Fvector& pos, const Fvector& vel)
{
	if(!pParticles) return;

	Fmatrix particles_pos; 
	particles_pos.set(XFORM());
	particles_pos.c.set(pos);
	//pParticles->UpdateParent(particles_pos,vel);
	pParticles->SetXFORM(particles_pos);

	
	if(!pParticles->IsAutoRemove() && !pParticles->IsLooped() 
		&& !pParticles->PSI_alive())
	{
		pParticles->Stop();
		pParticles->PSI_destroy();
		pParticles = NULL;
	}
}