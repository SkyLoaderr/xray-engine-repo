////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\ai_monsters_anims.h"

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
	CAniFVector	  <caInPlaceNames>						m_tInPlace;

	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256		S;
		m_tGlobal.Load	(tpKinematics,caBaseName);
		m_tTorso.Load	(tpKinematics,strconcat(S,caBaseName,"torso_"));
		m_tMoves.Load	(tpKinematics,caBaseName);
		m_tInPlace.Load	(tpKinematics,caBaseName);
	};
};

class CStalkerAnimations {
public:
	CAniCollection<CStateAnimations,caStateNames>	m_tAnims;
	CMotionDef		*m_tpCurrentGlobalAnimation;
	CMotionDef		*m_tpCurrentTorsoAnimation;
	CMotionDef		*m_tpCurrentLegsAnimation;
	CBlend			*m_tpCurrentGlobalBlend;
	CBlend			*m_tpCurrentTorsoBlend;
	CBlend			*m_tpCurrentLegsBlend;
	u8				m_bAnimationIndex;
	
					CStalkerAnimations()
	{
		m_tpCurrentGlobalAnimation = 
		m_tpCurrentTorsoAnimation = 
		m_tpCurrentLegsAnimation = 0;
	};
	
	virtual	void	Load(CKinematics *tpKinematics)
	{
		m_tAnims.Load	(tpKinematics,"");
	};
};