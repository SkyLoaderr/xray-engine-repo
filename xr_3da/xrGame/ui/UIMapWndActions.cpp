#include "stdafx.h"
#include "UIMapWndActions.h"
#include "UIMapWndActionsSpace.h"
#include "UIMap.h"
#include "UIMapWnd.h"

typedef CActionBase<CUIMapWnd>				WORLD_OPERATOR;

//actions
class CSomeMapAction : public WORLD_OPERATOR {
private:
	typedef WORLD_OPERATOR	inherited;
public:
					CSomeMapAction		(LPCSTR action_name):inherited((CUIMapWnd*)NULL,action_name){}
	virtual	void	initialize			()		{inherited::initialize	();};
	virtual	void	execute				()		{inherited::execute		();};
	virtual	void	finalize			()		{inherited::finalize	();};
};

class CMapActionZoomControl: public CSomeMapAction{
private:
	typedef CSomeMapAction	inherited;
protected:
	float			m_startMovingTime;
	float			m_endMovingTime;
	float			m_targetZoom;
	float			m_startZoom;
	Fvector2		m_startCenter;
public:
					CMapActionZoomControl	(LPCSTR action_name) : inherited(action_name) {}
	virtual	void	execute				();
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionResize : public CMapActionZoomControl {
private:
	typedef CMapActionZoomControl	inherited;
public:
					CMapActionResize	(LPCSTR action_name) : inherited(action_name) {}
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionMinimize : public CMapActionZoomControl {
private:
	typedef CMapActionZoomControl	inherited;
public:
					CMapActionMinimize	(LPCSTR action_name) : inherited(action_name) {}
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionCenter : public CMapActionZoomControl {
private:
	typedef CMapActionZoomControl	inherited;
public:
					CMapActionCenter	(LPCSTR action_name) : inherited(action_name) {}
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionIdle : public CSomeMapAction {
private:
	typedef CSomeMapAction	inherited;
public:
					CMapActionIdle		(LPCSTR action_name) : inherited(action_name) {}
	virtual	void	initialize			();
	virtual	void	execute				();
	virtual	void	finalize			();
};

//evaluators
class CSomeMapEvaluator : public CPropertyEvaluator<CUIMapWnd>
{
private:
	typedef CPropertyEvaluator<CUIMapWnd>	inherited;
public:
					CSomeMapEvaluator	(LPCSTR evaluator_name):inherited((CUIMapWnd*)NULL,evaluator_name){}
	virtual 		~CSomeMapEvaluator	()										{};
};

class CEvaluatorTargetMapShown : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorTargetMapShown(LPCSTR evaluator_name=0):inherited(evaluator_name){};
	virtual bool	evaluate			();
};

class CEvaluatorMapMinimized : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorMapMinimized	(LPCSTR evaluator_name=0):inherited(evaluator_name){};
	virtual bool	evaluate			();
};


class CEvaluatorMapResized : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
	CEvaluatorMapResized(LPCSTR evaluator_name=0):inherited(evaluator_name){};
	virtual bool	evaluate			();
};

class CEvaluatorMapConst: public CSomeMapEvaluator{
private:
	typedef CSomeMapEvaluator	inherited;
	bool	ret_value;
public:
					CEvaluatorMapConst	(bool val=false, LPCSTR evaluator_name=0):inherited(evaluator_name){ret_value=val;};
	virtual bool	evaluate			(){return ret_value;};

};

//-----------------------------------------------------------------------------
// action planner implementation
//-----------------------------------------------------------------------------

using namespace UIMapWndActionsSpace;
const float			map_changing_time			= 1.0f;//sec
const float			map_moving_speed			= 0.003f;

CMapActionPlanner::CMapActionPlanner	()
{}

CMapActionPlanner::~CMapActionPlanner	()
{}

LPCSTR CMapActionPlanner::object_name	() const
{
	return						("");
}


void CMapActionPlanner::setup		(CUIMapWnd *object)
{
#ifdef LOG_ACTION
	set_use_log						(false);
#endif
	inherited::setup				(object);
	clear							();
	m_storage.set_property		(1,false);
	m_storage.set_property		(2,false);
	add_evaluator				(ePropTargetMapShown,	xr_new<CEvaluatorTargetMapShown>("ePropTargetMapShown"));
	add_evaluator				(ePropMapMinimized,		xr_new<CEvaluatorMapMinimized>	("ePropMapMinimized"));
	add_evaluator				(ePropMapResized,		xr_new<CEvaluatorMapResized>	("ePropMapResized"));
	add_evaluator				(ePropMapIdle,			xr_new<CEvaluatorMapConst>		(false,"ePropMapIdle"));

	// final world state
	_world_operator* action		= xr_new<CMapActionIdle>("eOperatorMapIdle");
	add_condition				(action,ePropMapResized,	true);
	add_condition				(action,ePropTargetMapShown,true);
	add_condition				(action,ePropMapIdle,		false);
	add_effect					(action,ePropMapIdle,		true);
	add_operator				(eOperatorMapIdle,action);


	action						= xr_new<CMapActionResize>("eOperatorMapResize");
	add_condition				(action,ePropTargetMapShown,	true);
	add_condition				(action,ePropMapResized,		false);
	add_effect					(action,ePropMapResized,		true);
	add_operator				(eOperatorMapResize,			action);

	action						= xr_new<CMapActionMinimize>("eOperatorMapMinimize");
	add_condition				(action,ePropTargetMapShown,false);
	add_effect					(action,ePropTargetMapShown,true);
	add_operator				(eOperatorMapMinimize,		action);

}

//-----------------------------------------------------------------------------
void CMapActionZoomControl::initialize	()
{
	inherited::initialize		();
	VERIFY						(m_object->m_tgtMap);
	m_startMovingTime			= Device.fTimeGlobal;
	m_endMovingTime				= m_startMovingTime+1.0f;
	m_startZoom					= m_object->GlobalMap()->GetCurrentZoom();

	Frect vis_rect				= m_object->ActiveMapRect		();
	vis_rect.getcenter			(m_startCenter);
	m_startCenter.sub			(m_object->GlobalMap()->GetAbsolutePos());
	m_startCenter.div			(m_object->GlobalMap()->GetCurrentZoom());
}
void CMapActionZoomControl::finalize	()
{
	inherited::finalize();
	m_object->UpdateScroll	();
}

void CMapActionZoomControl::execute		()
{
	inherited::execute();
	CUIGlobalMap*			gm	= m_object->GlobalMap();
	float gt				= Device.fTimeGlobal;
	float k					= (clampr((gt-m_startMovingTime)/(m_endMovingTime-m_startMovingTime),0.0f,1.0f));
	float invk				= 1.f-k;
	
	float new_zoom			= m_startZoom*invk + m_targetZoom*k;

	Fvector2 newCP;

	newCP.x					= m_startCenter.x*invk + m_object->m_tgtCenter.x*k;
	newCP.y					= m_startCenter.y*invk + m_object->m_tgtCenter.y*k;

	newCP.mul				(gm->GetCurrentZoom());

	Frect rect;
	gm->CalcOpenRect		(newCP,rect,new_zoom);
	gm->SetWndRect          (rect); 
	gm->Update				();
	m_object->UpdateScroll	();

}
//-----------------------------------------------------------------------------

void CMapActionResize::initialize()
{
	inherited::initialize();
	m_targetZoom				= m_object->GetZoom();
}

void CMapActionResize::finalize		()
{
	inherited::finalize				();
}
//-----------------------------------------------------------------------------

void CMapActionMinimize::initialize()
{
	inherited::initialize();
	m_targetZoom				= m_object->GlobalMap()->GetMinZoom();
}

void CMapActionMinimize::finalize()
{
	inherited::finalize			();
}

//-----------------------------------------------------------------------------
void CMapActionCenter::initialize()
{
	inherited::initialize();
	m_targetZoom				= m_object->GetZoom();
}

void CMapActionCenter::finalize		()
{
	inherited::finalize				();
}
//-----------------------------------------------------------------------------
void CMapActionIdle::initialize	()
{
	inherited::initialize		();
	m_object->UpdateScroll		();
}
void CMapActionIdle::finalize	()
{
	inherited::finalize			();
}
void CMapActionIdle::execute	()
{
	inherited::execute			();
	m_storage->set_property	(1,true);
	m_storage->set_property	(2,false);
}
//-----------------------------------------------------------------------------

bool CEvaluatorTargetMapShown::evaluate()
{
	if(m_storage->property(1)) return true;
	if(m_storage->property(2)) return true;
	VERIFY(m_object->m_tgtMap);
	Fvector2 pt					= m_object->m_tgtCenter; 
	pt.mul						(m_object->GlobalMap()->GetCurrentZoom());
	pt.add						(m_object->GlobalMap()->GetAbsolutePos());
	if (m_object->ActiveMapRect().in(pt)){
		m_storage->set_property	(2,true);
		return true;
	}
	return false;
//	bool res = !!m_object->m_tgtMap->GetAbsoluteRect().intersected(m_object->ActiveMapRect());
//	return res;
}
//-----------------------------------------------------------------------------

bool CEvaluatorMapMinimized::evaluate	()
{
	if(m_storage->property(1)) return true;
	VERIFY(m_object->m_tgtMap);
	bool res = !!fsimilar(m_object->GlobalMap()->GetCurrentZoom(),m_object->GlobalMap()->GetMinZoom(),EPS_L );
	return res;
}
//-----------------------------------------------------------------------------


bool CEvaluatorMapResized::evaluate			()
{
	if(m_storage->property(1)) return true;
	VERIFY(m_object->m_tgtMap);
	bool res = !!fsimilar(m_object->GlobalMap()->GetCurrentZoom(),m_object->GetZoom(),EPS );
	return res;
}
