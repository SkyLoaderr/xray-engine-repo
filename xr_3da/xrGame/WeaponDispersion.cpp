// WeaponDispersion.cpp: разбос при стрельбе
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "weapon.h"
#include "inventoryowner.h"
#include "actor.h"

#include "actoreffector.h"
#include "effectorshot.h"


//возвращает 1, если оружие в отличном состоянии и >1 если повреждено
float CWeapon::GetConditionDispersionFactor() const
{
	return (1.f + fireDispersionConditionFactor*(1.f-GetCondition()));
}

float CWeapon::GetFireDispersion	(bool with_cartridge) const
{
	return GetFireDispersion	(with_cartridge?m_fCurrentCartirdgeDisp:1.f);
}

//текущая дисперсия (в радианах) оружия с учетом используемого патрона
float CWeapon::GetFireDispersion	(float cartridge_k) const
{
	//учет базовой дисперсии, состояние оружия и влияение патрона
	float fire_disp = fireDispersionBase*cartridge_k*GetConditionDispersionFactor();
	
	//вычислить дисперсию, вносимую самим стрелком
	const CInventoryOwner* pOwner	=	smart_cast<const CInventoryOwner*>(H_Parent());
	VERIFY (pOwner);

	float parent_disp = pOwner->GetWeaponAccuracy();
	fire_disp += parent_disp;

	return fire_disp;
}


//////////////////////////////////////////////////////////////////////////
// Для эффекта отдачи оружия
void CWeapon::AddShotEffector		()
{
	CActor* pActor = smart_cast<CActor*>(H_Parent());
	if(pActor)
	{
		CEffectorShot* S		= smart_cast<CEffectorShot*>	(pActor->EffectorManager().GetEffector(eCEShot)); 
		if (!S)	S				= (CEffectorShot*)pActor->EffectorManager().AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed, camMaxAngleHorz, camStepAngleHorz));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
}

void  CWeapon::RemoveShotEffector	()
{
	CActor* pActor = smart_cast<CActor*>(H_Parent());
	if(pActor)
		pActor->EffectorManager().RemoveEffector	(eCEShot);
}

const Fvector& CWeapon::GetRecoilDeltaAngle()
{
	CActor* pActor		= smart_cast<CActor*>(H_Parent());

	CEffectorShot* S = NULL;
	if(pActor)
		S = smart_cast<CEffectorShot*>(pActor->EffectorManager().GetEffector(eCEShot)); 

	if(S)
	{
		S->GetDeltaAngle(m_vRecoilDeltaAngle);
		return m_vRecoilDeltaAngle;
	}
	else
	{
		m_vRecoilDeltaAngle.set(0.f,0.f,0.f);
		return m_vRecoilDeltaAngle;
	}
}