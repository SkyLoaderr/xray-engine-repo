// Wound.h: класс описания раны
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_space.h"
#include "EntityCondition.h"

class CWound
{
public:
	CWound				(u16 bone_num);
	virtual ~CWound		(void);

	float	TotalSize	();
	float	TypeSize	(ALife::EHitType hit_type);

	void	AddHit		(float hit_power, ALife::EHitType hit_type);
	
	//заживление раны
	void	Incarnation	(float percent);
	u16		GetBoneNum	() {return m_iBoneNum;}
	void 	SetBoneNum	(u16 bone_num) {m_iBoneNum = bone_num;}

	u16		GetParticleBoneNum	() {return m_iParticleBoneNum;}
	void	SetParticleBoneNum	(u16 bone_num) {m_iParticleBoneNum = bone_num;}

	const ref_str& GetParticleName	()						{return m_sParticleName;}
	void	SetParticleName	(ref_str particle_name) {m_sParticleName = particle_name;}

protected:
	//косточка на которой появилась рана
	u16 m_iBoneNum;

	//косточка, если на ране отыгрывается партикл
	u16 m_iParticleBoneNum;
	//имя этого партикла
	ref_str m_sParticleName;

	//список составляющих раны 
	HitTypeSVec m_Wounds;
};