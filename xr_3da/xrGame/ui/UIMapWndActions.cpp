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
	Frect			m_desiredMapRect;
	bool			m_bZoomIn;
public:
					CMapActionZoomControl	(LPCSTR action_name) : inherited(action_name) {m_bZoomIn = true;}
	virtual	void	execute				();
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionMaximize : public CMapActionZoomControl {
private:
	typedef CMapActionZoomControl	inherited;
public:
					CMapActionMaximize	(LPCSTR action_name) : inherited(action_name) {m_bZoomIn = true;}
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionMinimize : public CMapActionZoomControl {
private:
	typedef CMapActionZoomControl	inherited;
public:
					CMapActionMinimize	(LPCSTR action_name) : inherited(action_name) {m_bZoomIn = false;}
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionCenter : public CSomeMapAction {
private:
	typedef CSomeMapAction	inherited;
protected:
	float			m_startMovingTime;
	float			m_endMovingTime;
	Frect			m_desiredMapRect;
public:
					CMapActionCenter	(LPCSTR action_name) : inherited(action_name) {}
	virtual	void	initialize			();
	virtual	void	execute				();
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

class CEvaluatorMapCentered : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorMapCentered(LPCSTR evaluator_name=0):inherited(evaluator_name){};
	virtual bool	evaluate			();
};

class CEvaluatorMapZoomed : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
	CEvaluatorMapZoomed(LPCSTR evaluator_name=0):inherited(evaluator_name){};
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
	set_use_log						(true);
#endif
	inherited::setup				(object);
	clear							();
	m_storage.set_property		(1,false);
	add_evaluator				(ePropFoo,				xr_new<CEvaluatorMapConst>		(false,"ePropFoo"));
	add_evaluator				(ePropTargetMapShown,	xr_new<CEvaluatorTargetMapShown>("ePropTargetMapShown"));
	add_evaluator				(ePropMapMinimized,		xr_new<CEvaluatorMapMinimized>	("ePropMapMinimized"));
	add_evaluator				(ePropMapCentered,		xr_new<CEvaluatorMapCentered>	("ePropMapCentered"));
	add_evaluator				(ePropMapIdle,			xr_new<CEvaluatorMapConst>		(false,"ePropMapIdle"));
	add_evaluator				(ePropMapZoomed,		xr_new<CEvaluatorMapZoomed>		("ePropMapZoomed"));

	// final world state
	_world_operator* action		= xr_new<CMapActionIdle>("eOperatorMapIdle");
	add_condition				(action,ePropMapZoomed,		true);
	add_condition				(action,ePropMapCentered,	true);
	add_condition				(action,ePropTargetMapShown,true);
	add_condition				(action,ePropMapIdle,		false);
	add_effect					(action,ePropMapIdle,		true);
	add_operator				(eOperatorMapIdle,action);


	action						= xr_new<CMapActionMaximize>("eOperatorMapZoomIn");
	add_condition				(action,ePropMapCentered,	true);
	add_condition				(action,ePropMapMinimized,	false);
	add_condition				(action,ePropTargetMapShown,true);
	add_condition				(action,ePropMapZoomed,		false);
	add_effect					(action,ePropMapMinimized,	false);
	add_effect					(action,ePropMapZoomed,		true);
	add_operator				(eOperatorMapZoomIn,action);

	action						= xr_new<CMapActionMinimize>("eOperatorMapZoomOut");
	add_condition				(action,ePropMapMinimized,	false);
	add_condition				(action,ePropTargetMapShown,false);
	add_effect					(action,ePropMapMinimized,	true);
	add_effect					(action,ePropTargetMapShown,true);
	add_operator				(eOperatorMapZoomOut,action);

	action						= xr_new<CMapActionCenter>("eOperatorMapCenter");
	add_condition				(action,ePropTargetMapShown,true);
	add_condition				(action,ePropMapCentered,	false);
	add_effect					(action,ePropMapCentered,	true);
	add_operator				(eOperatorMapCenter,action);
}

// actions and evaluators implementation
//-----------------------------------------------------------------------------
void calcRectLerp(float time_to, const Frect& desired_rect, Frect& current_rect)
{
	float gdt				= Device.fTimeDelta;
	gdt						= _min(gdt,time_to);

	current_rect.x1		+= ((desired_rect.x1-current_rect.x1)/time_to)*gdt;
	current_rect.y1		+= ((desired_rect.y1-current_rect.y1)/time_to)*gdt;
	current_rect.x2		+= ((desired_rect.x2-current_rect.x2)/time_to)*gdt;
	current_rect.y2		+= ((desired_rect.y2-current_rect.y2)/time_to)*gdt;
}

void CMapActionZoomControl::initialize	()
{
	VERIFY(m_object->m_tgtMap);
}
void CMapActionZoomControl::finalize	()
{
//	m_object->m_tgtMap	= NULL;
}
void CMapActionZoomControl::execute		()
{
	inherited::execute();
	CUIGlobalMap*			gm	= m_object->GlobalMap();
	if(m_endMovingTime > Device.fTimeGlobal){
		float gt				= Device.fTimeGlobal;
		float time_to			= m_endMovingTime-gt;

		Frect curr_map_rect		= gm->GetWndRect();
		calcRectLerp			(time_to,m_desiredMapRect,curr_map_rect);
		gm->SetWndRect			(curr_map_rect);
	}else{
		gm->SetWndRect			(m_desiredMapRect);
	}	
}
//-----------------------------------------------------------------------------

void CMapActionMaximize::initialize()
{
	inherited::initialize();
	
	Fvector2 destMapCP			= m_object->m_tgtMap->ConvertRealToLocalNoTransform(m_object->m_tgtMap->TargetCenter());
	destMapCP.add				(m_object->m_tgtMap->GetWndPos());

	m_object->GlobalMap()->CalcOpenRect(destMapCP,m_desiredMapRect, 16.0f);
	
	m_startMovingTime			= Device.fTimeGlobal;
	m_endMovingTime				= m_startMovingTime + map_changing_time;
}
void CMapActionMaximize::finalize		()
{
	inherited::finalize				();
}
//-----------------------------------------------------------------------------

void CMapActionMinimize::initialize()
{
	inherited::initialize();

	Fvector2 destMapCP			= m_object->m_tgtMap->ConvertRealToLocalNoTransform(m_object->m_tgtMap->TargetCenter());
	destMapCP.add				(m_object->m_tgtMap->GetWndPos());

	m_object->GlobalMap()->CalcOpenRect(destMapCP,m_desiredMapRect,m_object->GlobalMap()->GetMinZoom());

	m_startMovingTime			= Device.fTimeGlobal;
	m_endMovingTime				= m_startMovingTime + map_changing_time;
}
void CMapActionMinimize::finalize()
{
	inherited::finalize			();
}
//-----------------------------------------------------------------------------

void CMapActionIdle::initialize	()
{
	inherited::initialize		();
}
void CMapActionIdle::finalize	()
{
	inherited::finalize			();
}
void CMapActionIdle::execute	()
{
	inherited::execute			();
	m_storage->set_property	(1,true);

}
//-----------------------------------------------------------------------------

void CMapActionCenter::initialize()
{
	inherited::initialize		();

	Fvector2 destMapCP			= m_object->m_tgtMap->ConvertRealToLocalNoTransform(m_object->m_tgtMap->TargetCenter());
	destMapCP.add				(m_object->m_tgtMap->GetAbsolutePos());

	Frect	vis_abs_rect		= m_object->ActiveMapRect();
	Fvector2 tmp_pt2;
	vis_abs_rect.getcenter		(tmp_pt2);
	tmp_pt2.sub					(destMapCP);

	m_desiredMapRect			= m_object->GlobalMap()->GetWndRect();
	m_desiredMapRect.add		(tmp_pt2.x, tmp_pt2.y);

	m_startMovingTime			= Device.fTimeGlobal;
	m_endMovingTime				= m_startMovingTime + _sqrt(tmp_pt2.x*tmp_pt2.x+tmp_pt2.y*tmp_pt2.y)*map_moving_speed;
}

void CMapActionCenter::finalize	()
{
	inherited::finalize			();
}
void CMapActionCenter::execute	()
{
	inherited::execute			();

	CUIGlobalMap*			gm	= m_object->GlobalMap();
	if(m_endMovingTime > Device.fTimeGlobal){
		float gt				= Device.fTimeGlobal;
		float time_to			= m_endMovingTime-gt;

		Frect					curr_map_rect;
		curr_map_rect			= gm->GetWndRect();
		calcRectLerp			(time_to,m_desiredMapRect,curr_map_rect);
		gm->SetWndRect			(curr_map_rect);
	}else{
		gm->SetWndRect			(m_desiredMapRect);
	}
}
//-----------------------------------------------------------------------------
bool CEvaluatorTargetMapShown::evaluate()
{
	if(m_storage->property(1)) return true;
	VERIFY(m_object->m_tgtMap);
	bool res = !!m_object->m_tgtMap->GetAbsoluteRect().intersected(m_object->ActiveMapRect());
	return res;
}
//-----------------------------------------------------------------------------

bool CEvaluatorMapMinimized::evaluate	()
{
	if(m_storage->property(1)) return true;
	VERIFY(m_object->m_tgtMap);
	bool res = !!fsimilar(m_object->GlobalMap()->GetCurrentZoom(),m_object->GlobalMap()->GetMinZoom(),EPS );
	return res;
}
//-----------------------------------------------------------------------------

bool CEvaluatorMapCentered::evaluate	()
{
	if(m_storage->property(1)) return true;
	VERIFY(m_object->m_tgtMap);
	Fvector2 destMapCP			= m_object->m_tgtMap->ConvertRealToLocalNoTransform(m_object->m_tgtMap->TargetCenter());
	destMapCP.add				(m_object->m_tgtMap->GetAbsolutePos());

	Frect	vis_abs_rect		= m_object->ActiveMapRect();
	Fvector2 tmp_pt2;
	vis_abs_rect.getcenter		(tmp_pt2);
	tmp_pt2.sub					(destMapCP);
	return (_sqrt(tmp_pt2.x*tmp_pt2.x+tmp_pt2.y*tmp_pt2.y) < 3.0f);
}

bool CEvaluatorMapZoomed::evaluate			()
{
	if(m_storage->property(1)) return true;
	VERIFY(m_object->m_tgtMap);
	bool res = !!fsimilar(m_object->m_tgtMap->GetCurrentZoom(),m_object->GetZoom(),EPS );
	return res;
}
