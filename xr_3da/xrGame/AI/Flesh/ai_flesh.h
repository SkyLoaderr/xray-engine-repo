#pragma once

#include "..\\biting\\ai_biting.h"

class CAI_Flesh;

class CTest : public IState {
		CAI_Flesh	*pMonster;
public:
						CTest	(CAI_Flesh *p);
		virtual void	Run		();
};




class CAI_Flesh : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Flesh		();
	virtual					~CAI_Flesh		();	
	
	virtual void	Init					();
	virtual void	Load					(LPCSTR section);
	virtual void    StateSelector			();

	virtual	void	MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);
	virtual	void	LoadAttackAnim			();
	virtual	void	CheckAttackHit			();

			bool	ConeSphereIntersection	(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, 
											 Fvector SphereCenter, float SphereRadius);
			
	
// TEMP
	FvectorVec			PTurn;
	u32					PState;
	Fvector				PStart, PFinish;
	Fmatrix				MStart;
	TTime				PressedLastTime;
	
			void	MakeTurn();
			void	SetPoints();
			void	MoveInAxis(Fvector &Pos, const Fvector &dir,  float dx);
// ---

	CTest			*stateTest;	
	friend	class	CTest;

#ifdef DEBUG
	virtual void	OnRender				();
#endif

};

//
//
//class CFleshCapture : public IState {
//
//	CAI_Flesh	*pMonster;
//
//	enum {
//		ACTION_RUN,
//		ACTION_CAPTURE,
//		ACTION_CARRY_BACK,
//	} m_tAction;
//
//	CEntity			*pCorpse;
//	float			m_fDistToCorpse;			//!< дистанция до трупа
//
//	bool			flag_once_1;
//	
//	u32				SavedNodeID;
//	Fvector			SavedPosition;
//
//public:
//					CFleshCapture	(CAI_Flesh *p);
//	virtual	void	Reset			();
//private:
//	virtual void	Init			();
//	virtual void	Run				();
//};





