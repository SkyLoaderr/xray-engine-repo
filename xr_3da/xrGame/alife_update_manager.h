////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_update_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator update manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_switch_manager.h"
#include "alife_surge_manager.h"
#include "alife_storage_manager.h"

class CALifeUpdateManager :
	public CALifeSwitchManager,
	public CALifeSurgeManager,
	public CALifeStorageManager,
	public ISheduled
{
protected:
	bool				m_changing_level;
	u64					m_max_process_time;
	float				m_update_monster_factor;

protected:
			void		new_game				(LPCSTR	save_name);
			void		init_ef_storage			() const;
			void		update					(bool switch_objects);
	virtual	void		reload					(LPCSTR section);

public:
						CALifeUpdateManager		(xrServer *server, LPCSTR section);
	virtual 			~CALifeUpdateManager	();
	virtual float		shedule_Scale			();
	virtual void		shedule_Update			(u32 dt);	
	virtual BOOL		shedule_Ready			();
			void		load					(LPCSTR game_name = 0, bool no_assert = false);
			bool		load_game				(LPCSTR game_name, bool no_assert = false);
	IC		float		update_monster_factor	() const;
			bool		change_level			(NET_Packet	&net_packet);
			void		set_process_time		(int microseconds);
			void		set_switch_online		(ALife::_OBJECT_ID id, bool value);
			void		set_switch_offline		(ALife::_OBJECT_ID id, bool value);
			void		set_interactive			(ALife::_OBJECT_ID id, bool value);
			void		jump_to_level			(LPCSTR level_name) const;
			void		teleport_object			(ALife::_OBJECT_ID id, ALife::_GRAPH_ID game_vertex_id, u32 level_vertex_id, const Fvector &position);
};

#include "alife_update_manager_inline.h"