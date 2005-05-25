#ifndef ELEVATOR_STAETE
#define ELEVATOR_STAETE
class CPHCharacter;
struct dContact;
struct SGameMtl;
class CClimableObject;
class CElevatorState
{
	enum Estate
	{
		clbNone			,				
		clbNearUp		,			
		clbNearDown		,		
		clbClimbingUp	,		
		clbClimbingDown	,	
		clbDepart		,
		clbNoLader		
	};
Estate m_state;

CClimableObject	*m_ladder;	
CPHCharacter	*m_character;
Fvector			m_depart_position;//for depart state
u32				m_depart_time;
public: 
						CElevatorState					();
			void		PhTune							(float step)																			;
			void		SetCharacter					(CPHCharacter *character);
			void		SetElevator						(CClimableObject* climable);
			void		EvaluateState					();
			void		GetControlDir					(Fvector& dir);
			void		GetJumpDir						(const Fvector& accel,Fvector& dir);
			void		GetLeaderNormal					(Fvector& dir);
			bool		Active							(){return m_ladder && m_state!=clbNone;}
			bool		NearDown						(){return m_state == clbNearDown;}
			bool		NearState						(){return m_state==clbNearUp || m_state == clbNearDown;}
			bool		ClimbingState					(){return m_state==clbClimbingUp || m_state == clbClimbingDown;}
			void		Depart							();
			float		ClimbDirection					();
			void		Deactivate						();
private:
			void		InitDepart						();

			void		PhDataUpdate					(float step)																			;
			void		InitContact						(dContact* c,bool &do_collide,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)		;
			void		SwitchState						(Estate new_state);
			void		UpdateStNone					();
			void		UpdateStNearUp					();
			void		UpdateStNearDown				();
			void		UpdateStClimbingUp				();
			void		UpdateStClimbingDown			();
			void		UpdateClimbingCommon			(const Fvector	&d_to_ax,float to_ax,const Fvector& control_accel,float ca);
			void		UpdateDepart					();

			

};

#endif