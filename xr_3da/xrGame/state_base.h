////////////////////////////////////////////////////////////////////////////
//	Module 		: state_base.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Base state
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _Object
>
class CStateBase {
protected:
	_Object				*m_object;
	u32					m_start_level_time;
	ALife::_TIME_ID		m_start_game_time;
	u32					m_inertia_time;
public:
							CStateBase			();
	virtual					~CStateBase			();
			void			Init				();
	virtual	void			Load				(LPCSTR section);
	virtual	void			reinit				(_Object *object);
	virtual	void			reload				(LPCSTR section);
	virtual	void			initialize			();
	virtual	void			execute				();
	virtual	void			finalize			();
			bool			completed			() const;
	IC		u32				start_level_time	() const;
	IC		ALife::_TIME_ID	start_game_time		() const;
	IC		u32				inertia_time		() const;
	IC		void			set_inertia_time	(u32 inertia_time);
};

#include "state_base_inline.h"