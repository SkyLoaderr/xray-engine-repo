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
	
	// Just test
	CTest			*stateTest;	
	friend	class	CTest;

};




