////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_evaluators.h
//	Created 	: 24.04.2004
//  Modified 	: 24.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover evaluators
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level.h"

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBase
//////////////////////////////////////////////////////////////////////////

class CCoverPoint;

class CCoverEvaluatorBase {
protected:
	CCoverPoint			*m_selected;
	u32					m_last_update;
	u32					m_inertia_time;
	float				m_best_value;
	bool				m_initialized;
	Fvector				m_start_position;

public:
	IC					CCoverEvaluatorBase	();
	IC		CCoverPoint	*selected			() const;
	IC		bool		inertia				() const;
	IC		bool		initialized			() const;
	IC		void		setup				();
	IC		void		set_inertia			(u32 inertia_time);
	IC		void		initialize			(const Fvector &start_position);
	IC		void		finalize			();
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorCloseToEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorCloseToEnemy : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	Fvector				m_enemy_position;
	float				m_min_distance;
	float				m_max_distance;
	float				m_current_distance;
	float				m_deviation;
	float				m_best_distance;

public:
	IC		void		initialize			(const Fvector &start_position);
	IC		void		setup				(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, float deviation = 0.f);
			void		evaluate			(CCoverPoint *cover_point);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorFarFromEnemy : public CCoverEvaluatorCloseToEnemy {
protected:
	float				m_best_distance;
public:
			void		evaluate			(CCoverPoint *cover_point);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBest
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBest : public CCoverEvaluatorCloseToEnemy {
public:
			void		evaluate			(CCoverPoint *cover_point);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAngle
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorAngle : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

protected:
	Fvector				m_direction;
	Fvector				m_best_direction;
	float				m_best_alpha;
	u32					m_level_vertex_id;

public:
	IC		void		setup				(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, u32 level_vertex_id);
			void		initialize			(const Fvector &start_position);
			void		evaluate			(CCoverPoint *cover_point);
};

#include "cover_evaluators_inline.h"