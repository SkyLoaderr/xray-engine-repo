////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_location_predictor.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy location predictor
////////////////////////////////////////////////////////////////////////////

#pragma once

class CEnemyLocationPredictor {
public:
					CEnemyLocationPredictor			();
	virtual			~CEnemyLocationPredictor		();
			void	predict_enemy_location			();
			bool	enemy_prediction_actual			() const;
};