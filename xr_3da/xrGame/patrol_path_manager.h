////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager.h
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_object_location.h"
#include "ai_script_space.h"

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
	const CLevel::SPath							*m_path;
	EPatrolStartType							m_start_type;
	EPatrolRouteType							m_route_type;
	bool										m_actuality;
	bool										m_failed;
	bool										m_completed;
	bool										m_random;
	u32											m_curr_point_index;
	u32											m_prev_point_index;
	Fvector										m_dest_position;
	const luabind::functor<void>				*m_callback;
protected:
	IC			bool	actual						() const;
	IC			bool	completed					() const;
	IC			bool	failed						() const;
	IC			bool	random						() const;
	IC			const Fvector &destination_position	() const;
	IC			void	set_path					(const CLevel::SPath *path);
	IC			void	set_start_type				(const EPatrolStartType patrol_start_type);
	IC			void	set_route_type				(const EPatrolRouteType patrol_route_type);
	IC			void	set_random					(bool random);
				void	select_point				(const Fvector &position, u32 &dest_vertex_id);
public:
	IC					CPatrolPathManager			();
	IC	virtual			~CPatrolPathManager			();
	IC	virtual	void	Init						();
	IC	virtual	void	set_callback				(const luabind::functor<void> *callback);
};

#include "patrol_path_manager_inline.h"