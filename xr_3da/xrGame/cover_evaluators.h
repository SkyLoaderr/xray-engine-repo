////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_evaluators.h
//	Created 	: 24.04.2004
//  Modified 	: 24.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover evaluators
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level.h"
#include "restricted_object.h"

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
	CRestrictedObject	*m_object;

public:
	IC					CCoverEvaluatorBase	(CRestrictedObject *object);
	IC		CCoverPoint	*selected			() const;
	IC		bool		inertia				() const;
	IC		bool		initialized			() const;
	IC		void		setup				();
	IC		void		set_inertia			(u32 inertia_time);
	IC		void		initialize			(const Fvector &start_position);
	IC		void		finalize			();
	IC		bool		accessible			(const Fvector &position);
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
	IC					CCoverEvaluatorCloseToEnemy	(CRestrictedObject *object);
	IC		void		initialize			(const Fvector &start_position);
	IC		void		setup				(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, float deviation = 0.f);
			void		evaluate			(CCoverPoint *cover_point);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorFarFromEnemy : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

protected:
	float				m_best_distance;

public:
	IC					CCoverEvaluatorFarFromEnemy	(CRestrictedObject *object);
			void		evaluate			(CCoverPoint *cover_point);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBest
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBest : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

public:
	IC					CCoverEvaluatorBest	(CRestrictedObject *object);
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
	IC					CCoverEvaluatorAngle(CRestrictedObject *object);
	IC		void		setup				(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, u32 level_vertex_id);
			void		initialize			(const Fvector &start_position);
			void		evaluate			(CCoverPoint *cover_point);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorSafe
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorSafe : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	float				m_min_distance;

public:
	IC					CCoverEvaluatorSafe	(CRestrictedObject *object);
	IC		void		setup				(float min_distance);
			void		evaluate			(CCoverPoint *cover_point);
};

#include "cover_evaluators_inline.h"