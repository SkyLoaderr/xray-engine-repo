////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.h
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\ai_monsters_anims.h"
#include "ai_biting_space.h"

namespace AI_Biting {
	extern LPCSTR caStateNames		[];
	extern LPCSTR caGlobalNames		[];
};


#pragma todo("Oles to Jim: what does it mean? 'SelectAnimation(XFORM().k,XFORM().k,0)'")
#define FORCE_ANIMATION_SELECT() {\
	m_tpCurrentGlobalAnimation = 0; \
	SelectAnimation(XFORM().k,XFORM().k,0);\
}


class CBitingAnimations {
protected:
	CBitingAnimations()
	{
		m_tpCurrentGlobalAnimation = 0;
	};

	virtual	void	Load(CKinematics *tpKinematics)
	{
		m_tAnims.Load	(tpKinematics,"");
	};

protected:
	typedef CAniCollection<CAniVector,AI_Biting::caGlobalNames> CStateAnimations;
	CAniCollection<CStateAnimations,AI_Biting::caStateNames>	m_tAnims;
	u8				m_bAnimationIndex;

public:
	CMotionDef		*m_tpCurrentGlobalAnimation;
};


/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct tagAnimCell{
	AI_Biting::EActionAnim		m_tActionAnim;
	AI_Biting::EPostureAnim		m_tPostureAnim;
	bool						Locked;
	bool						PlayOnce;
	
	void Set( AI_Biting::EPostureAnim posture, AI_Biting::EActionAnim action, bool once, bool lock){
		m_tActionAnim = action;
		m_tPostureAnim = posture;
		Locked = lock;
		PlayOnce = once;
	}
} TAnimCell;


class CAnim {
	TAnimCell	DefState;
public:
	TAnimCell	CurState;

	typedef std::list<TAnimCell> TAnim;
	TAnim Anim;

	void Init(AI_Biting::EPostureAnim posture,AI_Biting::EActionAnim action) {		// 	инициализация состояния по умолчанию
		DefState.Set(posture,action,false,false);
		CurState = DefState;
	}

	void ChangeAnimation() {
		if (Anim.empty()) {
			//if (CurState.PlayOnce) CurState = DefState;
			CurState = DefState;
		} else {
			CurState = Anim.front();
			Anim.pop_front();
		}
	}

	void Set(AI_Biting::EPostureAnim posture,AI_Biting::EActionAnim action, bool once, bool lock, bool critical=false)
	{
		TAnimCell tCell;		
		tCell.Set(posture, action, lock, once);
		TAnimCell tempCell;

		if (critical && !CurState.Locked) {
			CurState = tCell; 
			Anim.clear();						
		} if (critical && CurState.Locked) {
			Anim.clear();
			Anim.push_back(tCell);
		} else {
			tempCell = Anim.back();
			if (tempCell.m_tPostureAnim != posture || tempCell.m_tActionAnim != action)
				if (Anim.size() < 5) // максимум 5 в стеке
					Anim.push_back(tCell);
		}
	}
};

