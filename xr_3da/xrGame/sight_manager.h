////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_graph.h"
#include "sight_action.h"

class CAI_Stalker;

class CSightManager {
protected:
	xr_vector<CSightAction*>	m_actions;
	CSightAction				*m_current_action;
	CAI_Stalker					*m_object;
	bool						m_actuality;

public:
					CSightManager						();
	virtual			~CSightManager						();
			void	Init								();
	virtual	void	Load								(LPCSTR section);
	virtual	void	reinit								(CAI_Stalker *object);
	virtual void	Exec_Look							(float dt);
			bool	bfIf_I_SeePosition					(Fvector tPosition) const;
			void	SetPointLookAngles					(const Fvector &tPosition, float &yaw, float &pitch);
			void	SetFirePointLookAngles				(const Fvector &tPosition, float &yaw, float &pitch);
			void	SetDirectionLook					();
			void	SetLessCoverLook					(const CLevelGraph::CVertex *tpNode, bool bDifferenceLook);
			void	SetLessCoverLook					(const CLevelGraph::CVertex *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook);
			void	vfValidateAngleDependency			(float x1, float &x2, float x3);
	IC		void	GetDirectionAnglesByPrevPositions	(float &yaw, float &pitch);
	IC		void	GetDirectionAngles					(float &yaw, float &pitch);

			void	update								(u32 time_delta);
	IC		bool	use_torso_look						() const;
	IC		CSightAction &current_action				() const;
	IC		void	clear								();
	IC		void	add_action							(CSightAction *action);
			void	select_action						();
			void	update								(const SightManager::ESightType &sight_type, const Fvector *vector3d = 0, u32 interval = u32(-1));
};

#include "sight_manager_inline.h"