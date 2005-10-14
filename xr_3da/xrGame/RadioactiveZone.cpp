#include "stdafx.h"
#include "radioactivezone.h"
#include "hudmanager.h"
#include "level.h"
#include "xrmessages.h"
#include "../bone.h"
#include "clsid_game.h"
#include "game_base_space.h"


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
		float power = (GameID() == GAME_SINGLE) ? Power(GO->Position().distance_to(pos)) : 0.0f;
		float impulse = 0.f;
		if(power > 0.01f) 
		{
			m_dwDeltaTime = 0;
			position_in_bone_space.set(0.f,0.f,0.f);

			CreateHit(GO->ID(),ID(),dir,power,BI_NONE,position_in_bone_space,impulse,ALife::eHitTypeRadiation);
		}
	}
}

void CRadioactiveZone::feel_touch_new					(CObject* O	)
{
	inherited::feel_touch_new(O);
	if (GameID() != GAME_SINGLE)
	{
		if (O->CLS_ID == CLSID_OBJECT_ACTOR)
		{
			CreateHit(O->ID(),ID(),Fvector().set(0, 0, 0),0.0f,BI_NONE,Fvector().set(0, 0, 0),0.0f,ALife::eHitTypeRadiation);
		}
	};
};

void CRadioactiveZone::UpdateWorkload					(u32	dt)
{
	if (IsEnabled() && GameID() != GAME_SINGLE)
	{	
		OBJECT_INFO_VEC_IT it;
		Fvector pos; 
		XFORM().transform_tiny(pos,CFORM()->getSphere().P);
		for(it = m_ObjectInfoMap.begin(); m_ObjectInfoMap.end() != it; ++it) 
		{
			if( !(*it).object->getDestroy() && (*it).object->CLS_ID == CLSID_OBJECT_ACTOR)
			{
				//=====================================
				NET_Packet	l_P;
				l_P.write_start();
				l_P.read_start();

				l_P.w_u16	(ID());
				l_P.w_u16	(ID());
				l_P.w_dir	(Fvector().set(0,0,0));
				float dist = (*it).object->Position().distance_to(pos);
				float power = Power(dist);
///				Msg("Zone Dist %f, Radiation Power %f, ", dist, power);
				l_P.w_float	(power/1000*dt);
				l_P.w_s16	(BI_NONE);
				l_P.w_vec3	(Fvector().set(0,0,0));
				l_P.w_float	(0.0f);
				l_P.w_u16	(ALife::eHitTypeRadiation);

				(*it).object->OnEvent(l_P, GE_HIT);
				//=====================================
			};
		}
	}
	inherited::UpdateWorkload(dt);
}