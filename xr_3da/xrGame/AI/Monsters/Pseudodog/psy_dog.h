#pragma once
#include "pseudodog.h"
#include "../../../script_export_space.h"

class CPsyDogPhantom;

class CPsyDog : public CAI_PseudoDog {
	typedef CAI_PseudoDog inherited;
public:
						CPsyDog				();
		virtual			~CPsyDog			();

		virtual void	Load				(LPCSTR section);
		virtual BOOL	net_Spawn			(CSE_Abstract *dc);

		virtual void	reinit				();
		virtual void	reload				(LPCSTR section);


				void	register_phantom	(CPsyDogPhantom*);
private:
				bool	spawn_phantom		();
				void	delete_phantom		(CPsyDogPhantom*);
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
public:
					CPsyDogPhantom		();
	virtual			~CPsyDogPhantom		();
	virtual BOOL	net_Spawn			(CSE_Abstract *dc);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPsyDogPhantom)
#undef script_type_list
#define script_type_list save_type_list(CPsyDogPhantom)


