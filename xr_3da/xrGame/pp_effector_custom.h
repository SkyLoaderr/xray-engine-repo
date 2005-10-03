#pragma once

#include "../effectorPP.h"
#include "../cameramanager.h"

class CPPEffectCustom : public CEffectorPP {
	typedef CEffectorPP inherited;

	SPPInfo		m_state;
	float		m_factor;

public:
					CPPEffectCustom		(const SPPInfo &ppi, EEffectorPPType type);
	virtual			~CPPEffectCustom	();

			void	update				(float new_factor) {m_factor = new_factor;}
			void	destroy				();

protected:
	virtual	BOOL	Process				(SPPInfo& pp);

};

class CPPEffectorCustom {
public:
				CPPEffectorCustom	();
	virtual 	~CPPEffectorCustom	();

	virtual void	load					(LPCSTR section);
	virtual void	update					(); 
	
	virtual bool	active					() {return (m_effector != 0);}

	virtual bool	check_completion		() = 0;
	virtual bool	check_start_conditions	() = 0;
	virtual void	update_factor			() = 0;

	virtual	void	activate				();
	virtual	void	deactivate				();

protected:
	float			m_factor;

private:
	SPPInfo			m_state;
	CPPEffectCustom	*m_effector;
};

