////////////////////////////////////////////////////////////////////////////
//	Module 		: autosave_manager.h
//	Created 	: 04.11.2004
//  Modified 	: 04.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Autosave manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAutosaveManager : public ISheduled {
private:
	typedef ISheduled inherited;

private:
	enum ESaveState {
		eSaveStateWaitForUpdate = u32(0),
		eSaveStateWaitForText,
		eSaveStateWaitForSend,
		eSaveStateWaitForDummy = u32(-1),
	};

private:
	u32				m_autosave_interval;
	u32				m_last_autosave_time;
	u32				m_delay_autosave_interval;
	u32				m_not_ready_count;
	ESaveState		m_save_state;

public:
					CAutosaveManager		();
	virtual			~CAutosaveManager		();
	virtual	void	shedule_Update			(u32 dt);
	virtual float	shedule_Scale			();
			void	OnRender				();

public:
	IC		u32		autosave_interval		() const;
	IC		u32		last_autosave_time		() const;
	IC		u32		not_ready_count			() const;
	IC		void	inc_not_ready			();
	IC		void	dec_not_ready			();
	IC		void	update_autosave_time	();
	IC		void	delay_autosave			();
	IC		bool	ready_for_autosave		();
};

#include "autosave_manager_inline.h"