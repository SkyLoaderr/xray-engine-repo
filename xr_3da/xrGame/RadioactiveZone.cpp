#include "stdafx.h"
#include "radioactivezone.h"
#include "hudmanager.h"
#include "level.h"
#include "xrmessages.h"


CRadioactiveZone::CRadioactiveZone(void) 
{
}

CRadioactiveZone::~CRadioactiveZone(void) 
{
}

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

void CRadioactiveZone::Affect(CObject* O) 
{
	CGameObject *GO = smart_cast<CGameObject*>(O);
	
	if(GO) 
	{
		Fvector pos; 
		XFORM().transform_tiny(pos,CFORM()->getSphere().P);
		char pow[255]; 
		sprintf(pow, "zone hit. %.1f", Power(GO->Position().distance_to(pos)));
		if(bDebug) HUD().outMessage(0xffffffff, GO->cName(), pow);
		
		Fvector dir; 
		dir.set(0,0,0);
	
		Fvector position_in_bone_space;
		float power = Power(GO->Position().distance_to(pos));
		float impulse = 0.f;
		if(power > 0.01f) 
		{
			m_dwDeltaTime = 0;
			position_in_bone_space.set(0.f,0.f,0.f);
			if (OnServer())
			{
				NET_Packet		P;
				CGameObject::u_EventGen	(P,GE_HIT,GO->ID());
				P.w_u16			(ID());
				P.w_u16			(ID());
				P.w_dir			(dir);
				P.w_float		(power);
				P.w_s16			(BI_NONE);
				P.w_vec3		(position_in_bone_space);
				P.w_float		(impulse);
				P.w_u16			(ALife::eHitTypeRadiation);
				CGameObject::u_EventSend	(P);
			};
		}
	}
}