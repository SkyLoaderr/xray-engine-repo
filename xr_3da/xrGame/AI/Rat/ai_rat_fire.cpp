////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_fire.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "ai_rat_impl.h"
#include "ai_rat_space.h"
#include "../../clsid_game.h"
#include "../../memory_manager.h"
#include "../../enemy_manager.h"
#include "../../item_manager.h"
#include "../../sound_player.h"

using namespace RatSpace;

void CAI_Rat::Exec_Action(float /**dt/**/)
{
	switch (m_tAction) {
		case eRatActionAttackBegin : {
			u32					dwTime = Device.dwTimeGlobal;
			sound().play	(eRatSoundAttack);//,0,0,m_dwHitInterval+1,m_dwHitInterval);
			if (memory().enemy().selected() && memory().enemy().selected()->g_Alive() && (dwTime - m_dwStartAttackTime > m_dwHitInterval)) {
				m_bActionStarted = true;
				m_dwStartAttackTime = dwTime;
				Fvector tDirection;
				Fvector position_in_bone_space;
				position_in_bone_space.set(0.f,0.f,0.f);
				tDirection.sub(memory().enemy().selected()->Position(),this->Position());
				vfNormalizeSafe(tDirection);
				
				if (this->Local() && memory().enemy().selected()) {
					CEntityAlive	*entity_alive = const_cast<CEntityAlive*>(memory().enemy().selected());
					VERIFY			(entity_alive);
					entity_alive->Hit(m_fHitPower,tDirection,this,0,position_in_bone_space,0);
				}
			}
			else
				m_bActionStarted = false;
			break;
		}
		case eRatActionAttackEnd : {
			m_bActionStarted = false;
			break;
		}
		default:
			break;
	}
}

void CAI_Rat::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 /**element/**/)
{
	// Save event
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
	m_hit_time = Device.dwTimeGlobal;
	m_hit_direction.set(D);
	m_hit_direction.normalize();
	m_tHitPosition = who->Position();
	
	// Play hit sound
	if (!AlreadyDie())
		sound().play		(eRatSoundInjuring);
}

bool CAI_Rat::useful		(const CGameObject *object) const
{
	if (!memory().item().useful(object))
		return			(false);

	const CEntityAlive	*entity_alive = smart_cast<const CEntityAlive*>(object);
	if (!entity_alive)
		return			(false);

	return				(true);
}

float CAI_Rat::evaluate		(const CGameObject *object) const
{
	const CEntityAlive	*entity_alive = smart_cast<const CEntityAlive*>(object);
	VERIFY				(entity_alive);
	if (!entity_alive->g_Alive()) {
		if ((Device.dwTimeGlobal - entity_alive->GetLevelDeathTime() < m_dwEatCorpseInterval) && (entity_alive->m_fFood > 0) && (m_bEatMemberCorpses || (entity_alive->g_Team() != g_Team())) && (m_bCannibalism || (entity_alive->CLS_ID != CLS_ID)))
			return		(entity_alive->m_fFood*entity_alive->m_fFood)*Position().distance_to(entity_alive->Position());
		else
			return		(flt_max);
	}
	else
		return			(flt_max);
}

void CAI_Rat::vfUpdateMorale()
{
	u32 dwCurTime = Device.dwTimeGlobal;
	if (m_fMorale < m_fMoraleMinValue)
		m_fMorale = m_fMoraleMinValue;
	if (m_fMorale > m_fMoraleMaxValue)
		m_fMorale = m_fMoraleMaxValue;
	if (dwCurTime - m_dwMoraleLastUpdateTime > m_dwMoraleRestoreTimeInterval) {
		m_dwMoraleLastUpdateTime = dwCurTime;
		float fDistance = Position().distance_to(m_tSafeSpawnPosition);
		fDistance = fDistance < 1.f ? 1.f : fDistance;
		switch (m_eCurrentState) {
			case aiRatFreeHuntingActive :
			case aiRatFreeHuntingPassive : {
				if (m_fMorale < m_fMoraleNormalValue) {
					m_fMorale += m_fMoraleRestoreQuant;//*(1.f - fDistance/m_fMoraleNullRadius);
					if (m_fMorale > m_fMoraleNormalValue)
						m_fMorale = m_fMoraleNormalValue;
				}
				else 
					if (m_fMorale > m_fMoraleNormalValue) {
						m_fMorale -= m_fMoraleRestoreQuant;//*(fDistance/m_fMoraleNullRadius);
						if (m_fMorale < m_fMoraleNormalValue)
							m_fMorale = m_fMoraleNormalValue;
					}
				break;
			}
			case aiRatUnderFire :
			case aiRatRetreat : {
				//m_fMorale += fDistance <= m_fMoraleNullRadius ? m_fMoraleRestoreQuant : 0;
				//m_fMorale += m_fMoraleRestoreQuant*(m_fMoraleNullRadius/fDistance);
				m_fMorale += m_fMoraleRestoreQuant;
				break;
			}
			case aiRatAttackRun :
			case aiRatAttackFire :
			case aiRatReturnHome : {
				//m_fMorale += m_fMoraleRestoreQuant*(1.f - fDistance/m_fMoraleNullRadius);
				//m_fMorale += m_fMoraleRestoreQuant*(m_fMoraleNullRadius/fDistance);
				m_fMorale += m_fMoraleRestoreQuant;
				break;
			}
		}
		if (m_fMorale < m_fMoraleMinValue)
			m_fMorale = m_fMoraleMinValue;
		if (m_fMorale > m_fMoraleMaxValue)
			m_fMorale = m_fMoraleMaxValue;
	}
}
