#pragma once

class CMonsterMovement : virtual public CMovementManager {
	
	CAI_Biting			*pMonster;

	bool				b_try_min_time;

public:
			CMonsterMovement		();
			~CMonsterMovement		();

	void	Load					();
	void	Init					();
			
	void	Frame_Init				();
	void	Frame_Update			();
	void	Frame_Finalize			();



};