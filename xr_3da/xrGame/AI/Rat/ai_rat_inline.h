////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_inline.h
//	Created 	: 23.04.2002
//  Modified 	: 26.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat" (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC void CAI_Rat::vfChangeGoal()
{
	Fvector vP;
	vP.set(m_tSpawnPosition.x,m_tSpawnPosition.y,m_tSpawnPosition.z);
	m_tGoalDir.x = vP.x+m_tVarGoal.x*::Random.randF(-0.5f,0.5f); 
	m_tGoalDir.y = vP.y+m_tVarGoal.y*::Random.randF(-0.5f,0.5f);
	m_tGoalDir.z = vP.z+m_tVarGoal.z*::Random.randF(-0.5f,0.5f);
}

IC bool CAI_Rat::bfCheckIfGoalChanged(bool bForceChangeGoal)
{
	if (m_fGoalChangeTime<=0){
		m_fGoalChangeTime += m_fGoalChangeDelta+m_fGoalChangeDelta*::Random.randF(-0.5f,0.5f);
		if (bForceChangeGoal)
			vfChangeGoal();
		return(true);
	}
	return(false);
};

IC void CAI_Rat::vfChooseNewSpeed()
{
	int iRandom = ::Random.randI(0,2);
	switch (iRandom) {
				case 0 : {
					m_fSpeed = m_fMaxSpeed;
					break;
						 }
				case 1 : {
					m_fSpeed = m_fMinSpeed;
					break;
						 }
	}
	m_fSafeSpeed = m_fSpeed;
};

IC void CAI_Rat::vfUpdateTime(float fTimeDelta)
{
	m_fGoalChangeTime -= fTimeDelta > .1f ? .1f : fTimeDelta;
};		

IC void CAI_Rat::vfAddActiveMember(bool bForceActive)
{
	CGroupHierarchyHolder &Group = Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group());
	if (!m_bActive && (bForceActive || (Group.m_dwAliveCount*m_dwActiveCountPercent/100 >= Group.m_dwActiveCount))) {
		m_bActive = true;
		m_eCurrentState = aiRatFreeHuntingActive;
		++Group.m_dwActiveCount;
		shedule.t_min	= m_dwActiveScheduleMin;
		shedule.t_max	= m_dwActiveScheduleMax;
		vfRemoveStandingMember();
	}
	//Msg("* Group : alive[%2d], active[%2d]",Group.m_dwAliveCount,Group.m_dwActiveCount);
};

IC void CAI_Rat::vfRemoveActiveMember()
{
	CGroupHierarchyHolder &Group = Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group());
	if (m_bActive) {
		R_ASSERT(Group.m_dwActiveCount > 0);
		--(Group.m_dwActiveCount);
		m_bActive = false;
		m_eCurrentState = aiRatFreeHuntingPassive;
		shedule.t_min	= m_dwPassiveScheduleMin;
		shedule.t_max	= m_dwPassiveScheduleMax;
	}
	//Msg("* Group : alive[%2d], active[%2d]",Group.m_dwAliveCount,Group.m_dwActiveCount);
};

IC void CAI_Rat::vfAddStandingMember()
{
	CGroupHierarchyHolder &Group = Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group());
	if ((Group.m_dwAliveCount*m_dwStandingCountPercent/100 >= Group.m_dwStandingCount) && (!m_bStanding)) {
		++Group.m_dwStandingCount;
		m_bStanding = true;
	}
};

IC void CAI_Rat::vfRemoveStandingMember()
{
	CGroupHierarchyHolder &Group = Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group());
	if (m_bStanding) {
		R_ASSERT(Group.m_dwStandingCount > 0);
		--(Group.m_dwStandingCount);
		m_bStanding = false;
	}
};

IC bool CAI_Rat::bfCheckIfSoundFrightful()
{
	return(((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_HIT) == SOUND_TYPE_WEAPON_BULLET_HIT) || ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING));
};

IC bool CAI_Rat::bfCheckIfOutsideAIMap(Fvector &tTemp1)
{
	u32 dwNewNode = level_vertex_id();
	const CLevelGraph::CVertex *tpNewNode = level_vertex();
	CLevelGraph::CPosition	QueryPos;
	if (!ai().level_graph().valid_vertex_position(tTemp1))
		return	(false);
	ai().level_graph().vertex_position(QueryPos,tTemp1);
	if (!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*level_vertex(),QueryPos)) {
		dwNewNode = ai().level_graph().vertex(level_vertex_id(),tTemp1);
		tpNewNode = ai().level_graph().vertex(dwNewNode);
	}
	return(!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*tpNewNode,QueryPos));
};

IC	bool CAI_Rat::use_model_pitch	() const
{
	return			(!!g_Alive());
}
