#ifndef ELEVATOR_STAETE
#define ELEVATOR_STAETE
class CPHCharacter;
struct dContact;
class SGameMtl;
class CClimableObject;
class CElevatorState
{
	enum
	{
		clbGetOn		=	1<<0,
		clbGetOut		=	1<<1,
		clbOnleader		=	1<<2,
		dirUp			=	1<<3,
		stContact		=	1<<4
	};
Flags8 m_flags;

CClimableObject	*m_ladder;																														;
public: 
						CElevatorState					(CPHCharacter* character)																;
private:
			void		PhTune							(float step)																			;
			void		PhDataUpdate					(float step)																			;
			void		InitContact						(dContact* c,bool &do_collide,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)		;
};

#endif