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

protected:
	bool			m_enabled;

public:
					CSightManager						();
	virtual			~CSightManager						();
			void	init								();
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
			void	GetDirectionAngles					(float &yaw, float &pitch);
	IC		bool	use_torso_look						() const;
	template <typename T1, typename T2, typename T3>
	IC		void	setup								(T1 _1, T2 _2, T3 _3);
	template <typename T1, typename T2>
	IC		void	setup								(T1 _1, T2 _2);
	template <typename T1>
	IC		void	setup								(T1 _1);
			void	setup								(const CSightAction &sight_action);
	virtual void	update								();
	IC		bool	enabled								() const;
	IC		void	enable								(bool value);
};

#include "sight_manager_inline.h"