////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_graph.h"
#include "ai_monster_space.h"

class CSightManager {
private:
	float					m_ls_yaw;
	u32						m_dwLookChangedTime;
	MonsterSpace::ELookType	m_tLookType;
public:
					CSightManager						();
	virtual			~CSightManager						();
			void	Init								();
	virtual	void	Load								(LPCSTR section);
	virtual	void	reinit								();
	virtual void	Exec_Look							(float dt);
			void	update								(MonsterSpace::ELookType tLookType, const Fvector	*tPointToLook = 0, u32 dwLookOverDelay = u32(-1));
			bool	bfIf_I_SeePosition					(Fvector tPosition) const;
			void	SetPointLookAngles					(const Fvector &tPosition, float &yaw, float &pitch);
			void	SetFirePointLookAngles				(const Fvector &tPosition, float &yaw, float &pitch);
			void	SetDirectionLook					();
			void	SetLessCoverLook					(const CLevelGraph::CVertex *tpNode, bool bDifferenceLook);
			void	SetLessCoverLook					(const CLevelGraph::CVertex *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook);
			void	vfValidateAngleDependency			(float x1, float &x2, float x3);
	IC		void	GetDirectionAnglesByPrevPositions	(float &yaw, float &pitch);
	IC		void	GetDirectionAngles					(float &yaw, float &pitch);
	IC		MonsterSpace::ELookType	look_type			() const;
};

#include "sight_manager_inline.h"