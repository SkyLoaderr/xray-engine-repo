////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

const u32 dwStateCount			= 2;
const u32 dwWeaponCount			= 4;
const u32 dwWeaponActionCount	= 7;
const u32 dwMovementCount		= 2;
const u32 dwMovementActionCount	= 4;
const u32 dwInPlaceCount		= 6;
const u32 dwGlobalCount			= 2;

extern LPCSTR caStateNames			[dwStateCount];
extern LPCSTR caWeaponNames			[dwWeaponCount];
extern LPCSTR caWeaponActionNames	[dwWeaponActionCount];
extern LPCSTR caMovementNames		[dwMovementCount];
extern LPCSTR caMovementActionNames	[dwMovementActionCount];
extern LPCSTR caInPlaceNames		[dwInPlaceCount];
extern LPCSTR caGlobalNames			[dwGlobalCount];

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

template <u32 COUNT, LPCSTR caBaseNames[]> class CAniFVector {
public:
	ANIM_VECTOR		A;
	
	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256 S;
		A.resize(COUNT);
		ANIM_IT		I = A.begin(), B = I;
		ANIM_IT		E = A.end();
		for ( ; I != E; I++)
			*I = tpKinematics->ID_Cycle(strconcat(S,caBaseName,caBaseNames[I - B]));
	}
};

template <typename TYPE_NAME, u32 COUNT, LPCSTR caBaseNames[]> class CAniCollection {
public:
	vector<TYPE_NAME*>	A;
	
	virtual			~CAniCollection()
	{
		free_vector(A);
	}

	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256 S;
		A.resize(COUNT);
		vector<TYPE_NAME*>::iterator	I = A.begin(), B = I;
		vector<TYPE_NAME*>::iterator	E = A.end();
		for ( ; I != E; I++) {
			*I = xr_new<TYPE_NAME>();
			(*I)->Load(tpKinematics,strconcat(S,caBaseName,caBaseNames[I - B]));
		}
	}
};

class CStateAnimations {
public:
	typedef CAniCollection<CAniVector,	dwMovementActionCount,	caMovementActionNames>	CMovementActions;
	typedef CAniCollection<CAniVector,	dwWeaponActionCount,	caWeaponActionNames>	CWeaponActions;
	
	// global
	CAniCollection<CAniVector,			dwGlobalCount,	caGlobalNames>		m_tGlobal;
	// torso
	CAniCollection<CWeaponActions,		dwWeaponCount,	caWeaponNames>		m_tTorso;
	// legs
	CAniCollection<CMovementActions,	dwMovementCount,caMovementNames>	m_tMoves;
	CAniFVector	  <dwInPlaceCount,		caInPlaceNames>						m_tInPlace;

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
	CAniCollection<CStateAnimations,dwStateCount,caStateNames>				m_tAnims;
	
	virtual			~CStalkerAnimations()
	{
	};

	virtual	void	Load(CKinematics *tpKinematics)
	{
		m_tAnims.Load	(tpKinematics,"");
	};
};