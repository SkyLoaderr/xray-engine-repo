#pragma once

#include "control_combase.h"
#include "../../movement_manager.h"

class CAbstractVertexEvaluator;
class CCustomMonster;
class CControl_Manager;

struct SControlPathBuilderData : public ControlCom::IComData {
	bool		use_dest_orientation;
	Fvector		dest_orientation;
	
	bool		try_min_time;

	Fvector		target_position;
	u32			target_node;

	bool		enable;
	bool		extrapolate;

	u32			velocity_mask;
	u32			desirable_mask;

	MovementManager::EPathType path_type;
};


class CControlPathBuilder : 
	public CControl_ComPure<SControlPathBuilderData>,
	public CMovementManager
{
	typedef CMovementManager							inherited;
	typedef CControl_ComPure<SControlPathBuilderData>	inherited_com;

	friend	class CControl_Manager;
	
	CAbstractVertexEvaluator	*m_selector_approach;

public:
					CControlPathBuilder		(CCustomMonster *monster);
	virtual			~CControlPathBuilder	();

	virtual void	load					(LPCSTR section);
	virtual void	reinit					();
	virtual void	update_schedule			();

	virtual	void	on_travel_point_change	(const u32 &previous_travel_point_index);
	virtual void	on_build_path			();

			// services
			bool	is_path_end				(float dist_to_end);
			bool	valid_destination		(const Fvector &pos, u32 node);
			bool	is_moving_on_path		();

			bool	get_node_in_radius		(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node);
			void	fix_position			(const Fvector &pos, u32 node, Fvector &res_pos);

private:	
			void	init_selector			(CAbstractVertexEvaluator *S, Fvector target_pos);
			bool	is_path_built			();
			bool	build_special			(const Fvector &target, u32 node, u32 vel_mask);
};
