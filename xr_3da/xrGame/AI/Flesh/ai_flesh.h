#pragma once
#include "../biting/ai_biting.h"

class CAI_Flesh : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Flesh		();
	virtual					~CAI_Flesh		();	
	
	virtual	void	Load					(LPCSTR section);
	virtual	BOOL	net_Spawn				(LPVOID DC);
	virtual void    StateSelector			();

	virtual	void	CheckSpecParams			(u32 spec_params);
	virtual void	ProcessTurn				();
	virtual u8		get_legs_number			() {return QUADRUPEDAL;}


	virtual bool	ability_can_drag		() {return true;}
	

	// debug
#ifdef DEBUG
	virtual void OnRender ();
#endif

	CBitingSearchEnemy	*stateSearchEnemy;

private:
	bool	ConeSphereIntersection	(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, 
									Fvector SphereCenter, float SphereRadius);
};




