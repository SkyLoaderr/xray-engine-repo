////////////////////////////////////////////////////////////////////////////
//	Module 		: state_base.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Base state
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAI_Stalker;

class CStateBase {
protected:
	CAI_Stalker			*m_object;
	u32					m_start_level_time;
	ALife::_TIME_ID		m_start_game_time;
	u32					m_inertia_time;
public:
							CStateBase			(CAI_Stalker *object);
	virtual					~CStateBase			();
			void			Init				();
	virtual	void			Load				(LPCSTR section);
	virtual	void			reinit				();
	virtual	void			reload				(LPCSTR section);
	virtual	void			initialize			();
	virtual	void			execute				();
	virtual	void			finalize			();
	IC		u32				start_level_time	() const;
	IC		ALife::_TIME_ID	start_game_time		() const;
	IC		u32				inertia_time		() const;
	IC		void			set_inertia_time	(u32 inertia_time);
};

#include "state_free_no_alife_inline.h"