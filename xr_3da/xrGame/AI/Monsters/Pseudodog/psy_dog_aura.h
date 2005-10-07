#pragma once
//////////////////////////////////////////////////////////////////////////
// Effector controlling class
//////////////////////////////////////////////////////////////////////////
#include "../../../pp_effector_custom.h"

//class CPPEffectorPsyDogAura : public CPPEffectorControlled {
//	typedef CPPEffectorControlled inherited;
//	
//	enum {eStateFadeIn, eStateFadeOut, eStatePermanent} m_effector_state;
//
//public:
//					CPPEffectorPsyDogAura	(const SPPInfo &ppi);
//	virtual BOOL	update					();
//			void	switch_off				();
//};
//
//
//
//class CPsyDog;
//class CActor;
//
//class CPsyDogAura : public CPPEffectorController {
//	typedef CPPEffectorController inherited;
//
//	CPsyDog			*m_object;
//	CActor			*m_actor;
//
//	float			m_time_switch;		// time_to_switch
//	
//public:
//			CPsyDogAura		(CPsyDog *dog) : m_object(dog){}
//
//	virtual void	load	(LPCSTR section);
//			void	reinit	();
//
//private:
//	virtual bool	check_completion				(){return false;}
//	virtual bool	check_start_conditions			(){return false;}
//	virtual void	update_factor					() {}
//	virtual void	frame_update					() {}
//	
//
//	virtual CPPEffectorControlled *create_effector	();
//};
//
