#pragma once
#include "../biting/ai_biting.h"

class CAI_Flesh : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Flesh		();
	virtual					~CAI_Flesh		();	
	
			void	Init					();
	virtual	void	Load					(LPCSTR section);
	virtual	BOOL	net_Spawn				(LPVOID DC);
	virtual void    StateSelector			();

	virtual	bool	AA_CheckHit				();

	virtual	void	CheckSpecParams			(u32 spec_params);

	// debug
#ifdef DEBUG
	virtual void OnRender ();
#endif

	CBitingSearchEnemy	*stateSearchEnemy;

			void	Testing					();
private:
	bool	ConeSphereIntersection	(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, 
									Fvector SphereCenter, float SphereRadius);
};




