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


#define FORCE_ANIMATION_SELECT() {\
	m_tpCurrentGlobalAnimation = 0; \
	SelectAnimation(clTransform.k,mRotate.k,0);\
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
	
	void Set(AI_Biting::EActionAnim action, AI_Biting::EPostureAnim posture, bool lock){
		m_tActionAnim = action;
		m_tPostureAnim = posture;
		Locked = lock;
	}
} TAnimCell;


class CAnim {
	bool bPlaying;	
	TAnimCell	DefState;
	TAnimCell	CurState;
public:

	typedef std::list<TAnimCell> TAnim;
	TAnim Anim;

	void Init(AI_Biting::EActionAnim action, AI_Biting::EPostureAnim posture) {		// 	инициализация состояния по умолчанию
		DefState.Set(action,posture,false);
		CurState = DefState;
		bPlaying = false;
	}

	void Play() {
		bPlaying = true;
	}
	
	void ChangeAnimation() {
		if (Anim.empty()) {
			CurState = DefState;
		} else {
			CurState = Anim.front();
			Anim.pop_front();
		}
	}

	void Stop() {
		bPlaying = false;
		CurState = DefState;		
	}

	void Set(AI_Biting::EActionAnim action, AI_Biting::EPostureAnim posture, bool lock, bool critical=false)
	{
		TAnimCell tCell;		
		tCell.Set(action, posture, lock);

		if (critical && !CurState.Locked) {
			CurState = tCell; 
			Anim.clear();						
		} if (critical && CurState.Locked) {
			Anim.clear();
			Anim.push_back(tCell);
		} else {
			Anim.push_back(tCell);
		}
	}
};

