// hit_immunity.h: класс для тех объектов, которые поддерживают
//				   коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"


//раны полученные различными типами хитов
typedef	svector<float,ALife::eHitTypeMax> HitTypeSVec;

class CHitImmunity
{
public:
	CHitImmunity();
	virtual ~CHitImmunity();

	virtual void LoadImmunities (LPCSTR section);
	virtual void LoadImmunities (LPCSTR section, LPCSTR line);
	virtual void InitImmunities (LPCSTR section,CInifile* ini);

	virtual float AffectHit		(float power, ALife::EHitType hit_type);

protected:
	//коэффициенты на которые домножается хит
	//при соответствующем типе воздействия
	//(для защитных костюмов и специфичных животных)
	HitTypeSVec m_HitTypeK;
};