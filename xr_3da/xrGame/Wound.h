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

protected:
	//косточка на которой появилась рана
	u16 m_iBoneNum;
	//косточка, если на ране отыгрывается партикл
	u16 m_iParticleBoneNum;
	//список составляющих раны 
	HitTypeSVec m_Wounds;
};