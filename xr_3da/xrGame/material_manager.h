////////////////////////////////////////////////////////////////////////////
//	Module 		: material_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Material manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CMaterialManager {
private:
	//в режиме бега подставляем другие звуки
	bool			m_run_mode;

	float			m_time_to_step;
	u32				m_step_id;
	u16				m_my_material_idx;
	ref_sound		m_step_sound[4];
protected:
	u16				m_last_material_idx;
public:
					CMaterialManager		();
	virtual			~CMaterialManager		();
			void	Init					();
	virtual void	Load					(LPCSTR section);
	virtual void	reinit					();
	virtual void	reload					(LPCSTR section);
	virtual CPHMovementControl*	movement_control()=0;
	virtual void	set_run_mode			(bool run_mode) {m_run_mode = run_mode;}

	virtual void	update					(float time_delta, float volume, float step_time, bool standing);
	IC		u16		last_material_idx		() const;
	IC		u16		self_material_idx		() const;
			
	IC SGameMtlPair	*get_current_pair		();
};

#include "material_manager_inline.h"
