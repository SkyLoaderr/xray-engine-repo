#include "stdafx.h"
#include "monster_cover_manager.h"
#include "BaseMonster/base_monster.h"
#include "../../cover_evaluators.h"
#include "../../cover_point.h"
#include "../../ai_space.h"
#include "../../level_navigation_graph.h"
#include "../../game_graph.h"
#include "../../game_level_cross_table.h"
#include "../../level.h"
#include "../../level_debug.h"
#include "../../cover_manager.h"

//////////////////////////////////////////////////////////////////////////
// CControllerCoverEvaluator
class CCoverEvaluator : public CCoverEvaluatorBase {
	typedef CCoverEvaluatorBase inherited;

	Fvector				m_dest_position;
	float				m_min_distance;
	float				m_max_distance;
	float				m_current_distance;
	float				m_deviation;
	float				m_best_distance;

public:
				CCoverEvaluator	(CRestrictedObject *object);

	// setup by cover_manager
	void		initialize		(const Fvector &start_position);

	// manual setup
	void		setup			(const Fvector &position, float min_pos_distance, float	max_pos_distance, float deviation = 0.f);

	void		evaluate		(CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CControllerCoverPredicate
class CCoverPredicate {
public:
	// setup internals here
				CCoverPredicate				();
	// called from cover_manager for every cover (for suitable cover)
	bool		operator()					(CCoverPoint *cover) const;
	// must return a value that is transfered to cover evaluator
	float		weight						(CCoverPoint *cover) const;

	void		finalize					(CCoverPoint *cover) const;
};


//////////////////////////////////////////////////////////////////////////
// CCoverEvaluator Implementation
//////////////////////////////////////////////////////////////////////////

CCoverEvaluator::CCoverEvaluator(CRestrictedObject *object) : inherited(object)
{
	m_dest_position.set		(flt_max,flt_max,flt_max);
	m_deviation				= flt_max;
	m_min_distance			= flt_max;
	m_max_distance			= flt_max;
	m_best_distance			= flt_max;
	m_current_distance		= flt_max;
}

void CCoverEvaluator::setup(const Fvector &position, float min_pos_distance, float max_pos_distance, float deviation)
{
	inherited::setup();

	m_dest_position			= position;

	m_actuality				= m_actuality && fsimilar(m_deviation,deviation);
	m_deviation				= deviation;

	m_actuality				= m_actuality && fsimilar(m_min_distance,min_pos_distance);
	m_min_distance			= min_pos_distance;

	m_actuality				= m_actuality && fsimilar(m_max_distance,max_pos_distance);
	m_max_distance			= max_pos_distance;

}

void CCoverEvaluator::initialize(const Fvector &start_position)
{
	inherited::initialize	(start_position);
	m_current_distance		= m_start_position.distance_to(m_dest_position);
#ifdef DEBUG
	DBG().level_info(this).clear();
#endif
}


void CCoverEvaluator::evaluate(CCoverPoint *cover_point, float weight)
{
#ifdef DEBUG
	//DBG().level_info(this).add_item(cover_point->position(), D3DCOLOR_XRGB(0,255,0));
#endif

	if (fis_zero(weight))
		return;

	float					dest_distance	= m_dest_position.distance_to(cover_point->position());

	if ((dest_distance <= m_min_distance) && (m_current_distance > dest_distance))
		return;

	if((dest_distance >= m_max_distance) && (m_current_distance < dest_distance))
		return;

	Fvector					direction;
	float					y,p;
	direction.sub			(m_dest_position,cover_point->position());
	direction.getHP			(y,p);

	float					cover_value = ai().level_graph().cover_in_direction(y,cover_point->level_vertex_id());
	float					value = cover_value;
	if (ai().level_graph().neighbour_in_direction(direction,cover_point->level_vertex_id()))
		value				+= 10.f;

	value					/= weight;

	if (value >= m_best_value)
		return;

	m_selected				= cover_point;
	m_best_value			= value;
}

//////////////////////////////////////////////////////////////////////////
// CCoverPredicate Implementation
//////////////////////////////////////////////////////////////////////////


bool CCoverPredicate::operator()				(CCoverPoint *cover) const
{
	return				(true);
}

float CCoverPredicate::weight					(CCoverPoint *cover) const
{
	return				(1.f);
}

void CCoverPredicate::finalize				(CCoverPoint *cover) const
{
}


//=============================================================================
// Cover Manager
//=============================================================================

CMonsterCoverManager::CMonsterCoverManager(CBaseMonster *monster) : m_object(monster)
{
}

void CMonsterCoverManager::load()
{
	m_ce_best = xr_new<CCoverEvaluator>(&(m_object->control().path_builder().restrictions()));
}

CCoverPoint *CMonsterCoverManager::find_cover(const Fvector &position, float min_pos_distance, float max_pos_distance, float deviation)
{
	m_ce_best->setup	(position,min_pos_distance,max_pos_distance,deviation);
	CCoverPoint			*point = ai().cover_manager().best_cover(m_object->Position(),30.f,*m_ce_best);
	
	return				point;
}


