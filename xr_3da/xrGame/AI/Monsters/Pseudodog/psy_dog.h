#pragma once
#include "pseudodog.h"
#include "../../../script_export_space.h"

class CPsyDogPhantom;

class CPsyDog : public CAI_PseudoDog {
	typedef CAI_PseudoDog inherited;

	friend class CPsyDogPhantom;

	// эффектор у актера при нахождении в поле
	//CPPEffectorDistance	m_aura_effector;

	// enemy transfered from phantom
	CActor *m_enemy;
public:
						CPsyDog				();
		virtual			~CPsyDog			();

		virtual void	Load				(LPCSTR section);
		virtual BOOL	net_Spawn			(CSE_Abstract *dc);
		virtual void	reinit				();
		virtual void	reload				(LPCSTR section);
		virtual void	net_Destroy			();
		virtual void	Die					(CObject* who);

		virtual void	Think				();
//				void	on_phantom_appear	();
		
private:
				bool	spawn_phantom		();
				void	delete_phantom		(CPsyDogPhantom*);
				void	register_phantom	(CPsyDogPhantom*);

				void	delete_all_phantoms	();

private:
	xr_vector<CPsyDogPhantom*> m_storage;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPsyDog)
#undef script_type_list
#define script_type_list save_type_list(CPsyDog)

//////////////////////////////////////////////////////////////////////////
// Phantom Psy Dog
//////////////////////////////////////////////////////////////////////////

class CPsyDogPhantom : public CAI_PseudoDog {
	typedef CAI_PseudoDog inherited;
	
	CPsyDog			*m_parent;

	enum {
		eWaitToAppear,
		eAttack
	}m_state;

	SAttackEffector m_appear_effector;

	LPCSTR			m_particles_appear;
	LPCSTR			m_particles_disappear;

public:
					CPsyDogPhantom		();
	virtual			~CPsyDogPhantom		();
	virtual BOOL	net_Spawn			(CSE_Abstract *dc);
	virtual void	Think				();
	virtual void	HitSignal			(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void	Hit					(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);

	virtual void	net_Destroy			();
	virtual void	Die					(CObject* who);

	IC		void	mark_destroyed		(){m_parent = 0;}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPsyDogPhantom)
#undef script_type_list
#define script_type_list save_type_list(CPsyDogPhantom)


