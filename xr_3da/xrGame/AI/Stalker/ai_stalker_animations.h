////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

extern LPCSTR caStateNames			[];
extern LPCSTR caWeaponNames			[];
extern LPCSTR caWeaponActionNames	[];
extern LPCSTR caMovementNames		[];
extern LPCSTR caMovementActionNames	[];
extern LPCSTR caInPlaceNames		[];
extern LPCSTR caGlobalNames			[];

DEFINE_VECTOR	(CMotionDef*,ANIM_VECTOR, ANIM_IT);

class CAniVector {
public:
	ANIM_VECTOR		A;

	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256 S1, S2;
		for (int i=0; ; i++) {
			CMotionDef *tpMotionDef = tpKinematics->ID_Cycle_Safe(strconcat(S1,caBaseName,itoa(i,S2,10)));
			if (tpMotionDef)
				A.push_back(tpMotionDef);
			else
				break;
		}
	}
};

template <LPCSTR caBaseNames[]> class CAniFVector {
public:
	ANIM_VECTOR		A;
	
	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256 S;
		for (int i=0; caBaseNames[i]; i++)
			A.push_back	(tpKinematics->ID_Cycle(strconcat(S,caBaseName,caBaseNames[i])));
	}
};

template <class TYPE_NAME, LPCSTR caBaseNames[]> class CAniCollection {
public:
	svector<TYPE_NAME,16>	A;
	
	virtual			~CAniCollection()
	{
		//free_vector(A);
	}

	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256 S;
		for (int i=0; caBaseNames[i]; i++) {
			A.resize	(i + 1);
			A[i].Load	(tpKinematics,strconcat(S,caBaseName,caBaseNames[i]));
		}
	}
};

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

	virtual			~CStateAnimations()
	{
	};

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
	CAniCollection<CStateAnimations,caStateNames>				m_tAnims;
	CMotionDef*		m_tpCurrentGlobalAnimation;
	CMotionDef*		m_tpCurrentTorsoAnimation;
	CMotionDef*		m_tpCurrentLegsAnimation;
	
					CStalkerAnimations()
	{
		m_tpCurrentGlobalAnimation = 
		m_tpCurrentTorsoAnimation = 
		m_tpCurrentLegsAnimation = 0;
	};
	
	virtual			~CStalkerAnimations()
	{
	};

	virtual	void	Load(CKinematics *tpKinematics)
	{
		m_tAnims.Load	(tpKinematics,"");
	};
};