////////////////////////////////////////////////////////////////////////////
//	Module 		: material_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Material manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamemtllib.h"

class CPHMovementControl;

class CMaterialManager {
private:
	bool						m_run_mode;
	float						m_time_to_step;
	u32							m_step_id;
	u16							m_my_material_idx;
	ref_sound					m_step_sound[4];
	CObject						*m_object;

protected:
	u16							m_last_material_idx;

public:
								CMaterialManager		();
	virtual						~CMaterialManager		();
	virtual DLL_Pure			*_construct				();
	virtual void				Load					(LPCSTR section);
	virtual void				reinit					();
	virtual void				reload					(LPCSTR section);
	virtual CPHMovementControl	*movement_control		() = 0;
	virtual void				set_run_mode			(bool run_mode);
	virtual void				update					(float time_delta, float volume, float step_time, bool standing);
	IC		u16					last_material_idx		() const;
	IC		u16					self_material_idx		() const;
	IC		SGameMtlPair		*get_current_pair		();
};

#include "material_manager_inline.h"