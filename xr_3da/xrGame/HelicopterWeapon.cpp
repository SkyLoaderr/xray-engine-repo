#include "stdafx.h"
#include "helicopter.h"

void __stdcall 
CHelicopter::BoneCallbackX(CBoneInstance *B)
{
	Fmatrix rX;		rX.rotateX		(PI_DIV_6);
	B->mTransform.mulB(rX);
}

void __stdcall 
CHelicopter::BoneCallbackY(CBoneInstance *B)
{
	Fmatrix rY;		rY.rotateY		(0.0f);
	B->mTransform.mulB(rY);
}


const Fmatrix&	
CHelicopter::ParticlesXFORM() const
{
	return m_fire_bone_xform;
}

IRender_Sector*	
CHelicopter::Sector()
{
	return 0;
}

const Fvector&	
CHelicopter::CurrentFirePoint()
{
	return m_fire_pos;
}


void 
CHelicopter::UpdateFire()
{

	fTime -= Device.fTimeDelta;


	CShootingObject::UpdateFlameParticles();
	CShootingObject::UpdateLight();

	if(!IsWorking()) 
	{
		if(fTime<0) fTime = 0.f;
		return;
	}

	if(fTime<=0)
	{
		OnShot();
		fTime += fTimeToFire;
	}

}
void 
CHelicopter::OnShot		()
{
//	VERIFY(Owner());

	FireBullet(CurrentFirePoint(),m_fire_dir, 
		fireDispersionBase,
		m_CurrentAmmo, /*Owner()->*/ID(),ID());

	StartFlameParticles();
	StartSmokeParticles(m_fire_pos, zero_vel);
	OnShellDrop(m_fire_pos, zero_vel);

/*	bool hud_mode = (Level().CurrentEntity() == dynamic_cast<CObject*>(Owner()));
	HUD_SOUND::PlaySound(sndShot, fire_pos, Owner(), hud_mode);

	//добавить эффектор стрельбы
	AddShotEffector		();
*/
}

void 
CHelicopter::FireStart()
{
	CShootingObject::FireStart();
}

void 
CHelicopter::FireEnd()
{
	CShootingObject::FireEnd();
	StopFlameParticles	();
//	RemoveShotEffector ();
}
