////////////////////////////////////////////////////////////////////////////
//	Module 		: location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

class CLocationSelector {
protected:
public:
					CLocationSelector		();
	virtual			~CLocationSelector		();
	virtual void	init_evaluator			(CAbstractNodeEvaluator *node_evaluator);
			void	select_location			(CAbstractNodeEvaluator *node_evaluator);
};