///////////////////////////////////////////////////////////////
// BastArtifact.h
// BastArtifact - артефакт мочалка
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"
#include "..\feel_touch.h"

#include "entity.h"


DEFINE_VECTOR (CEntityAlive*, ALIVE_LIST, ALIVE_LIST_it);


class CBastArtifact : public CArtifact,
					  public Feel::Touch
{
private:
	typedef CArtifact inherited;
public:
	CBastArtifact(void);
	virtual ~CBastArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();
	virtual void shedule_Update		(u32 dt);

	virtual	void Hit				(float P, Fvector &dir,	
										CObject* who, s16 element,
										Fvector position_in_object_space, 
										float impulse, 
										ALife::EHitType hit_type = eHitTypeWound);


	virtual void feel_touch_new	(CObject* O);
	virtual void feel_touch_delete	(CObject* O);
	virtual BOOL feel_touch_contact	(CObject* O);

protected:
	//параметры артефакта
	
	//пороговое значение импульса после получени€ 
	//которого артефакт активизируетс€
	float m_fImpulseThreshold;
	float m_fEnergy;
	float m_fRadius;
	float m_fStrikeImpulse;

	//флаг, того что артефакт получил хит 
	//и теперь может совершить бросок
	bool m_bStrike;	

	//список живых существ в зоне дос€гаемости артефакта
	ALIVE_LIST m_AliveList;
	//та жива€ сущность, которубю атакуем в данный момент
	CEntityAlive* m_pAttackingEntity; 
};
