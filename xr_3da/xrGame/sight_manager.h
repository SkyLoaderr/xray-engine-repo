////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_graph.h"
#include "setup_manager.h"
#include "sight_control_action.h"

class CAI_Stalker;

class CSightManager : public CSetupManager<CSightControlAction,CAI_Stalker,u32> {
public:
	typedef CSetupManager<CSightControlAction,CAI_Stalker,u32> inherited;

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
			bool	need_correction						(float x1, float x2, float x3);
	IC		void	GetDirectionAnglesByPrevPositions	(float &yaw, float &pitch);
	IC		void	GetDirectionAngles					(float &yaw, float &pitch);
	IC		bool	use_torso_look						() const;
			void	setup								(const SightManager::ESightType &sight_type, const Fvector *vector3d = 0, u32 interval = u32(-1));
			void	setup								(const CSightAction &sight_action);
	virtual void	update								(u32 time_delta);
};

#include "sight_manager_inline.h"