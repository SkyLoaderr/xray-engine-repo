////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../ai_monsters_anims.h"

extern LPCSTR caStateNames			[];
extern LPCSTR caWeaponNames			[];
extern LPCSTR caWeaponActionNames	[];
extern LPCSTR caMovementNames		[];
extern LPCSTR caMovementActionNames	[];
extern LPCSTR caInPlaceNames		[];
extern LPCSTR caGlobalNames			[];
extern float  faTurnAngles			[];


class CStateAnimations {
public:
	typedef CAniCollection<CAniVector,	caMovementActionNames>	CMovementActions;
	typedef CAniCollection<CAniVector,	caWeaponActionNames>	CWeaponActions;
	
	// global
	CAniCollection<CAniVector,		caGlobalNames>		m_tGlobal;
	// torso
	CAniCollection<CWeaponActions,	caWeaponNames>		m_tTorso;
	// legs
	CAniCollection<CMovementActions,caMovementNames>	m_tMoves;
	// in-place 
	CAniFVector	  <caInPlaceNames>						m_tInPlace;
	// global item
	CAniCollection<CWeaponActions,	caWeaponNames>		m_tGlobalItem;

	IC	void		Load(CSkeletonAnimated *tpKinematics, LPCSTR caBaseName)
	{
		string256			S;
		m_tGlobal.Load		(tpKinematics,caBaseName);
		m_tTorso.Load		(tpKinematics,strconcat(S,caBaseName,"torso_"));
		m_tMoves.Load		(tpKinematics,caBaseName);
		m_tInPlace.Load		(tpKinematics,caBaseName);
		m_tGlobalItem.Load	(tpKinematics,strconcat(S,caBaseName,"item_"));
	};
};

class CStalkerAnimations {
public:
	struct CScriptAnimation {
		bool		m_hand_usage;
		CMotionDef	*m_motion;

		IC			CScriptAnimation	(bool hand_usage, CMotionDef *motion) :
						m_hand_usage(hand_usage),
						m_motion	(motion)
		{
		}
	};

	CAniCollection<CStateAnimations,caStateNames>	m_tAnims;
	CMotionDef					*m_tpCurrentGlobalAnimation;
	CMotionDef					*m_tpCurrentTorsoAnimation;
	CMotionDef					*m_tpCurrentLegsAnimation;
	CMotionDef					*m_current_script_animation;
	CBlend						*m_tpCurrentGlobalBlend;
	CBlend						*m_tpCurrentTorsoBlend;
	CBlend						*m_tpCurrentLegsBlend;
	u8							m_bAnimationIndex;
	u32							m_dwDirectionStartTime;
	u32							m_dwAnimationSwitchInterval;
	EMovementDirection			m_tMovementDirection;
	EMovementDirection			m_tDesirableDirection;
	xr_deque<CScriptAnimation>	m_script_animations;
	IRender_Visual				*m_visual;
	
							CStalkerAnimations				()
	{
	};

	virtual	void			reinit							()
	{
		m_tpCurrentGlobalAnimation		= 
		m_tpCurrentTorsoAnimation		= 
		m_tpCurrentLegsAnimation		= 0;
		m_dwAnimationSwitchInterval		= 500;
		m_dwDirectionStartTime			= 0;
		m_tMovementDirection			= eMovementDirectionForward;
		m_tDesirableDirection			= eMovementDirectionForward;
		m_script_animations.clear		();
	}
	
	virtual	void			reload							(IRender_Visual *Visual, CInifile *ini, LPCSTR section)
	{
		m_visual								= Visual;
		m_tAnims.Load							(PSkeletonAnimated(Visual),"");
		int										head_bone = PKinematics(Visual)->LL_BoneID(ini->r_string(section,"bone_head"));
		PKinematics(Visual)->LL_GetBoneInstance	(u16(head_bone)).set_callback(HeadCallback,this);

		int										shoulder_bone = PKinematics(Visual)->LL_BoneID(ini->r_string(section,"bone_shoulder"));
		PKinematics(Visual)->LL_GetBoneInstance	(u16(shoulder_bone)).set_callback(ShoulderCallback,this);

		int										spin_bone = PKinematics(Visual)->LL_BoneID(ini->r_string(section,"bone_spin"));
		PKinematics(Visual)->LL_GetBoneInstance	(u16(spin_bone)).set_callback(SpinCallback,this);
	};
	
	static	void __stdcall	HeadCallback					(CBoneInstance *B);
	static	void __stdcall	ShoulderCallback				(CBoneInstance *B);
	static	void __stdcall	SpinCallback					(CBoneInstance *B);
			void			vfAssignGlobalAnimation			(CMotionDef *&tpGlobalAnimation);
			void			vfAssignTorsoAnimation			(CMotionDef *&tpGlobalAnimation);
			void			vfAssignLegsAnimation			(CMotionDef *&tpLegsAnimation);
			void			add_animation					(LPCSTR animation, bool hand_usage = false);
			void			clear_animations				();
};