////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_fire.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"

void CAI_Rat::Exec_Action(float /**dt/**/)
{
	switch (m_tAction) {
		case eRatActionAttackBegin : {
			u32 dwTime = Level().timeServer();
			if (enemy() && (enemy()->g_Health() > 0) && (dwTime - m_dwStartAttackTime > m_dwHitInterval)) {
				bool bOk = true;

				for (int i=0; i<SND_ATTACK_COUNT; ++i)
					if (m_tpaSoundAttack[i].feedback) {
						bOk = false;
						break;
					}

				if (bOk)
					m_tpaSoundAttack[Random.randI(SND_ATTACK_COUNT)].play_at_pos(this,Position());

				m_bActionStarted = true;
				m_dwStartAttackTime = dwTime;
				Fvector tDirection;
				Fvector position_in_bone_space;
				position_in_bone_space.set(0.f,0.f,0.f);
				tDirection.sub(enemy()->Position(),this->Position());
				vfNormalizeSafe(tDirection);
				
				if ((this->Local()) && enemy() && (CLSID_ENTITY == enemy()->CLS_ID)) {
					CEntityAlive	*entity_alive = const_cast<CEntityAlive*>(enemy());
					VERIFY			(entity_alive);
					entity_alive->Hit(m_fHitPower,tDirection,this,0,position_in_bone_space,0);
				}
			}
			else {
				if (enemy()) {
					bool bOk = true;

					for (int i=0; i<SND_ATTACK_COUNT; ++i)
						if (m_tpaSoundAttack[i].feedback) {
							bOk = false;
							break;
						}

					if (bOk && !::Random.randI(100))
						m_tpaSoundAttack[Random.randI(SND_ATTACK_COUNT)].play_at_pos(this,Position());
				}
				m_bActionStarted = false;
			}

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
	m_hit_time = Level().timeServer();
	m_hit_direction.set(D);
	m_hit_direction.normalize();
	m_tHitPosition = who->Position();
	
	// Play hit-ref_sound
	ref_sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		::Sound->play_at_pos		(S,this,Position());
	}
	if (g_Health() - amount <= 0) {
		if ((m_tpCurrentGlobalAnimation) && (!m_tpCurrentGlobalBlend->playing))
			if (m_tpCurrentGlobalAnimation != m_tRatAnimations.tNormal.tGlobal.tpaDeath[0])
				m_tpCurrentGlobalBlend = PSkeletonAnimated(Visual())->PlayCycle(m_tpCurrentGlobalAnimation = m_tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)]);
	}
}

bool CAI_Rat::useful		(const CGameObject *object) const
{
	if (!CItemManager::useful(object))
		return			(false);

	const CEntityAlive	*entity_alive = dynamic_cast<const CEntityAlive*>(object);
	if (!entity_alive)
		return			(false);

	return				(true);
}

float CAI_Rat::evaluate		(const CGameObject *object) const
{
	const CEntityAlive	*entity_alive = dynamic_cast<const CEntityAlive*>(object);
	VERIFY				(entity_alive);
	if (!entity_alive->g_Alive()) {
		if ((Level().timeServer() - entity_alive->GetLevelDeathTime() < m_dwEatCorpseInterval) && (entity_alive->m_fFood > 0) && (m_bEatMemberCorpses || (entity_alive->g_Team() != g_Team())) && (m_bCannibalism || (entity_alive->SUB_CLS_ID != SUB_CLS_ID)))
			return		(entity_alive->m_fFood*entity_alive->m_fFood)*Position().distance_to(entity_alive->Position());
		else
			return		(flt_max);
	}
	else
		return			(flt_max);
}

void CAI_Rat::vfUpdateMorale()
{
	u32 dwCurTime = Level().timeServer();
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

void CAI_Rat::vfUpdateMoraleBroadcast(float fValue, float /**fRadius/**/)
{
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	for (int i=0; i<(int)Group.Members.size(); ++i)
		if (Group.Members[i]->g_Alive())
			Group.Members[i]->m_fMorale += fValue;
}