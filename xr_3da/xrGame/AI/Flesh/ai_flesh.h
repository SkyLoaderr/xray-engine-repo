#pragma once

#include "..\\biting\\ai_biting.h"

class CAI_Flesh : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Flesh		();
	virtual					~CAI_Flesh		();	
	
	virtual void	Init					();

	virtual void	Think					();
	virtual void	UpdateCL				();

	virtual	void	MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);
	virtual	void	LoadAttackAnim			();
	virtual	void	CheckAttackHit			();

			bool	ConeSphereIntersection	(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, 
											 Fvector SphereCenter, float SphereRadius);

	// Flesh-specific FSM
	CBitingRest			*stateRest;
	CBitingAttack		*stateAttack;
	CBitingEat			*stateEat;
	CBitingHide			*stateHide;
	CBitingDetour		*stateDetour;
	CBitingPanic		*statePanic;
	CBitingExploreDNE	*stateExploreDNE;
	CBitingExploreDE	*stateExploreDE;
	CBitingExploreNDE	*stateExploreNDE;

	friend	class		CBitingRest;
	friend  class 		CBitingAttack;
	friend	class 		CBitingEat;
	friend	class 		CBitingHide;
	friend	class 		CBitingDetour;
	friend	class 		CBitingPanic;
	friend	class 		CBitingExploreDNE;
	friend	class 		CBitingExploreDE;
	friend	class 		CBitingExploreNDE;

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

#ifdef DEBUG
	virtual void	OnRender				();
#endif

};

