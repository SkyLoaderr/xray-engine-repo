#pragma once
#include "../effectorPP.h"

//////////////////////////////////////////////////////////////////////////

class CPPEffectorCustom : public CEffectorPP {
	typedef CEffectorPP inherited;
public:
					CPPEffectorCustom	(const SPPInfo &ppi, bool one_instance = false, bool destroy_from_engine = true);
	EEffectorPPType	get_type			(){return m_type;}

protected:
	virtual	BOOL	Process				(SPPInfo& pp);

	// update factor; if return FALSE - destroy
	virtual BOOL	update				(){return TRUE;}

private:
	SPPInfo			m_state;
	EEffectorPPType	m_type;
protected:
	float			m_factor;
};

//////////////////////////////////////////////////////////////////////////

class CPPEffectorController;

class CPPEffectorControlled : public CPPEffectorCustom {
	typedef CPPEffectorCustom inherited;

	CPPEffectorController	*m_controller;
public:
					CPPEffectorControlled	(CPPEffectorController *controller, const SPPInfo &ppi, bool one_instance = false, bool destroy_from_engine = true);
	virtual BOOL	update					();
	IC		void	set_factor				(float value){m_factor = value;}
};

//////////////////////////////////////////////////////////////////////////


class CPPEffectorController {
public:
				CPPEffectorController		();
	virtual 	~CPPEffectorController		();

	virtual void	load					(LPCSTR section);
	virtual void	frame_update			(); 
	
	virtual bool	active					() {return (m_effector != 0);}

	virtual bool	check_completion		() = 0;
	virtual bool	check_start_conditions	() = 0;
	virtual void	update_factor			() = 0;
	
	// factory method
	virtual CPPEffectorControlled *create_effector	() = 0;

protected:
			void	activate				();
			void	deactivate				();

protected:
	SPPInfo					m_state;
	CPPEffectorControlled	*m_effector;
};

