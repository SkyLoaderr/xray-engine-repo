////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_location_predictor.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy location predictor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "group.h"

class CEnemyLocationPredictor {
	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousPoints;
	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousForces;

public:
	SuspiciousPoints		m_tpaSuspiciousPoints;
	SuspiciousForces		m_tpaSuspiciousForces;

					CEnemyLocationPredictor			();
	virtual			~CEnemyLocationPredictor		();
			void	Init							();
	virtual void	reinit							();
			void	predict_enemy_locations			();
			bool	enemy_prediction_actual			() const;
			void	select_enemy_location			();
			bool	prediction_failed				() const;
};