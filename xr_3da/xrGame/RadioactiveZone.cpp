#include "stdafx.h"
#include "radioactivezone.h"
#include "hudmanager.h"
#include "level.h"
#include "xrmessages.h"


CRadioactiveZone::CRadioactiveZone(void) 
{}

CRadioactiveZone::~CRadioactiveZone(void) 
{}

void CRadioactiveZone::Load(LPCSTR section) 
{
	inherited::Load(section);
}

bool  CRadioactiveZone::BlowoutState	()
{
	bool result = inherited::BlowoutState();
	if(!result) UpdateBlowout();
	return result;
}

void CRadioactiveZone::Affect(SZoneObjectInfo* O) 
{
	// вермя срабатывания не чаще, чем заданный период
	if(m_dwDeltaTime < m_dwPeriod) return;
	m_dwDeltaTime = 0;
	
	CGameObject *GO = O->object;
	
	if(GO) 
	{
		Fvector pos; 
		XFORM().transform_tiny(pos,CFORM()->getSphere().P);

#ifdef DEBUG		
		char pow[255]; 
		sprintf(pow, "zone hit. %.1f", Power(GO->Position().distance_to(pos)));
		if(bDebug) HUD().outMessage(0xffffffff, *GO->cName(), pow);
#endif

		Fvector dir; 
		dir.set(0,0,0);
	
		Fvector position_in_bone_space;
		float power = Power(GO->Position().distance_to(pos));
		float impulse = 0.f;
		if(power > 0.01f) 
		{
			m_dwDeltaTime = 0;
			position_in_bone_space.set(0.f,0.f,0.f);

			CreateHit(GO->ID(),ID(),dir,power,BI_NONE,position_in_bone_space,impulse,ALife::eHitTypeRadiation);
		}
	}
}