////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager.h
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_object_location.h"
#include "ai/script/ai_script_monster.h"

class CPatrolPathManager : 
	virtual public CAI_ObjectLocation
{
public:
	enum EPatrolStartType {
		ePatrolStartTypeFirst = u32(0),
		ePatrolStartTypeLast,
		ePatrolStartTypeNearest,
		ePatrolStartTypeDummy = u32(-1),
	};
	enum EPatrolRouteType {
		ePatrolRouteTypeStop = u32(0),
		ePatrolRouteTypeContinue,
		ePatrolRouteTypeDummy = u32(-1),
	};
private:
	const CLevel::SPath		*m_path;
	ref_str					m_path_name;
	EPatrolStartType		m_start_type;
	EPatrolRouteType		m_route_type;
	bool					m_actuality;
	bool					m_failed;
	bool					m_completed;
	bool					m_random;
	u32						m_curr_point_index;
	u32						m_prev_point_index;
	Fvector					m_dest_position;
	const SMemberCallback	*m_callback;
protected:
	IC			bool	actual						() const;
	IC			bool	completed					() const;
	IC			bool	failed						() const;
	IC			bool	random						() const;
	IC			const Fvector &destination_position	() const;
	IC			void	set_path					(const CLevel::SPath *path, ref_str path_name);
				void	select_point				(const Fvector &position, u32 &dest_vertex_id);
public:
	IC					CPatrolPathManager			();
	IC	virtual			~CPatrolPathManager			();
	IC			void	Init						();
	IC	virtual	void	reinit						();
	IC	virtual	void	set_callback				(const SMemberCallback &callback);
	IC			void	make_inactual				();
	
	// for test only
	const CLevel::SPath* get_path					() {return m_path;}
	IC			void	set_path					(ref_str path_name);
	IC			void	set_path					(ref_str path_name, const EPatrolStartType patrol_start_type = ePatrolStartTypeNearest, const EPatrolRouteType patrol_route_type = ePatrolRouteTypeContinue, bool random = true);
	IC			void	set_start_type				(const EPatrolStartType patrol_start_type);
	IC			void	set_route_type				(const EPatrolRouteType patrol_route_type);
	IC			void	set_random					(bool random);
};

#include "patrol_path_manager_inline.h"