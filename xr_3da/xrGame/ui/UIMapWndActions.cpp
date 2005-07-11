#include "stdafx.h"
#include "UIMapWndActions.h"
#include "UIMapWndActionsSpace.h"
#include "UIMapWnd.h"

#include "../property_evaluator_const.h"

using namespace UIMapWndActionsSpace;

typedef CPropertyEvaluatorConst<CUIMapWnd>	CONST_EVALUATOR;
typedef CActionBase<CUIMapWnd>				WORLD_OPERATOR;

class CSomeAction : public WORLD_OPERATOR {
private:
	typedef WORLD_OPERATOR	inherited;

public:
					CSomeAction	(CUIMapWnd *object, LPCSTR action_name = "") : inherited(object,action_name) {}
	virtual	void	initialize	();
	virtual	void	execute		();
	virtual	void	finalize	();
};

void CSomeAction::initialize			()
{
	inherited::initialize	();
}

void CSomeAction::finalize				()
{
	inherited::finalize		();
}

void CSomeAction::execute				()
{
	inherited::execute		();
}

CMapActionPlanner::CMapActionPlanner	()
{
}

CMapActionPlanner::~CMapActionPlanner	()
{
}

LPCSTR CMapActionPlanner::object_name	() const
{
	return				("");
}

void CMapActionPlanner::setup			(CUIMapWnd *object)
{
	inherited::setup	(object);
	clear				();
	add_evaluators		();
	add_actions			();

	CWorldState					target_state;
	target_state.add_condition	(CWorldProperty(eWorldProperty1,true));
	set_target_state			(target_state);
}

void CMapActionPlanner::add_evaluators	()
{
	add_evaluator		(eWorldProperty1,	xr_new<CONST_EVALUATOR>(false,"eWorldProperty1"));
}

void CMapActionPlanner::add_actions		()
{
	_world_operator		*action = xr_new<CSomeAction>((CUIMapWnd*)NULL,"eWorldOperator1");
	add_condition		(action,eWorldProperty1,false);
	add_effect			(action,eWorldProperty1,true);
	add_operator		(eWorldOperator1,action);
}