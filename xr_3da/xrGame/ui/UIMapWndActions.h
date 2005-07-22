#pragma once
#include "../action_planner.h"
#include "../property_evaluator_const.h"

class CUIMapWnd;
class CUICustomMap;
class CUILevelMap;


class CMapActionPlanner :public CActionPlanner<CUIMapWnd,true> {
private:
	typedef CActionPlanner<CUIMapWnd,true> inherited;

public:
					CMapActionPlanner	();
	virtual			~CMapActionPlanner	();
	virtual	void	setup				(CUIMapWnd *object);
			void	add_evaluators		(u16 map_idx, CUILevelMap* map);
			void	add_actions			(u16 map_idx, CUILevelMap* map);
	virtual	LPCSTR	object_name			() const;
};

//typedef CPropertyEvaluatorConst<CUIMapWnd>	CONST_EVALUATOR;
typedef CActionBase<CUIMapWnd>				WORLD_OPERATOR;

//actions
class CSomeMapAction : public WORLD_OPERATOR {
private:
	typedef WORLD_OPERATOR	inherited;
protected:
	CUILevelMap*		m_map;
	shared_str			name;
public:
					CSomeMapAction		(CUILevelMap *object, u16 idx, LPCSTR action_name);
	virtual	void	initialize			()		{inherited::initialize	();};
	virtual	void	execute				()		{inherited::execute		();};
	virtual	void	finalize			()		{inherited::finalize	();};
};

class CMapActionOpenClose: public CSomeMapAction{
private:
	typedef CSomeMapAction	inherited;
protected:
	float			m_startMovingTime;
	float			m_endMovingTime;
	Frect			m_desiredLevelMapRect;
	Frect			m_desiredGlobalMapRect;
	bool			m_bOpening;
public:
					CMapActionOpenClose		(CUILevelMap *object, u16 idx, LPCSTR action_name) : inherited(object, idx, action_name) {}
	virtual	void	execute					();

};

class CMapActionOpen : public CMapActionOpenClose {
private:
	typedef CMapActionOpenClose	inherited;
public:
					CMapActionOpen		(CUILevelMap *object, u16 idx, LPCSTR action_name) : inherited(object, idx, action_name) {m_bOpening = true;}
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionClose : public CMapActionOpenClose {
private:
	typedef CMapActionOpenClose	inherited;
public:
					CMapActionClose		(CUILevelMap *object, u16 idx, LPCSTR action_name) : inherited(object, idx, action_name) {m_bOpening = false;}
	virtual	void	initialize			();
	virtual	void	finalize			();
};

class CMapActionLocalMapCenter : public CSomeMapAction {
private:
	typedef CSomeMapAction	inherited;
protected:
	float			m_startMovingTime;
	float			m_endMovingTime;
	Frect			m_desiredLevelMapRect;
public:
					CMapActionLocalMapCenter	(CUILevelMap *object, u16 idx, LPCSTR action_name) : inherited(object, idx, action_name) {}
	virtual	void	initialize					();
	virtual	void	execute						();
	virtual	void	finalize					();
};

class CMapActionGlobalMapCenter : public CSomeMapAction {
private:
	typedef CSomeMapAction	inherited;
protected:
	float			m_startMovingTime;
	float			m_endMovingTime;
	Frect			m_desiredGlobalMapRect;
public:
					CMapActionGlobalMapCenter	(CUILevelMap *object, u16 idx, LPCSTR action_name) : inherited(object, idx, action_name) {}
	virtual	void	initialize					();
	virtual	void	execute						();
	virtual	void	finalize					();
};

class CMapActionLocalMapZoom : public CSomeMapAction {
private:
	typedef CSomeMapAction	inherited;
protected:
	float			m_startMovingTime;
	float			m_endMovingTime;
	Frect			m_desiredLocalMapRect;
public:
					CMapActionLocalMapZoom	(CUILevelMap *object, u16 idx, LPCSTR action_name) : inherited(object, idx, action_name) {}
	virtual	void	initialize					();
	virtual	void	execute						();
	virtual	void	finalize					();
};

class CMapActionMapIdle : public CSomeMapAction {
private:
	typedef CSomeMapAction	inherited;
public:
					CMapActionMapIdle	(CUILevelMap *object, u16 idx, LPCSTR action_name) : inherited(object, idx, action_name) {}
	virtual	void	initialize					();
	virtual	void	execute						();
	virtual	void	finalize					();
};


//evaluators

class CSomeMapEvaluator : public CPropertyEvaluator<CUIMapWnd>
{
private:
	typedef CPropertyEvaluator<CUIMapWnd>	inherited;

protected:
		CUICustomMap*		m_map;
		shared_str			name;
public:
					CSomeMapEvaluator	(CUICustomMap *object = 0, u16 idx=0, LPCSTR evaluator_name=0);
	virtual 		~CSomeMapEvaluator	()										{};
};


class CEvaluatorMapOpened : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorMapOpened	(CUICustomMap *object = 0, u16 idx=0, LPCSTR evaluator_name=0):inherited(object,idx,evaluator_name){};
	virtual bool	evaluate			();
};

class CEvaluatorGlobalMapCentered : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorGlobalMapCentered	(CUICustomMap *object = 0, u16 idx=0, LPCSTR evaluator_name=0):inherited(object,idx,evaluator_name){};
	virtual bool	evaluate					();
};

class CEvaluatorLocalMapCentered : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorLocalMapCentered	(CUICustomMap *object = 0, u16 idx=0, LPCSTR evaluator_name=0):inherited(object,idx,evaluator_name){};
	virtual bool	evaluate					();
};

class CEvaluatorGlobalMapOpened : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorGlobalMapOpened	(CUICustomMap *object = 0, u16 idx=0, LPCSTR evaluator_name=0):inherited(object,idx,evaluator_name){};
	virtual bool	evaluate					();
};

class CEvaluatorLocalMapZoom : public CSomeMapEvaluator {
private:
	typedef CSomeMapEvaluator	inherited;
public:
					CEvaluatorLocalMapZoom	(CUICustomMap *object = 0, u16 idx=0, LPCSTR evaluator_name=0):inherited(object,idx,evaluator_name){};
	virtual bool	evaluate					();
};

class CMapEvaluatorConst: public CSomeMapEvaluator{
private:
	typedef CSomeMapEvaluator	inherited;
	bool	ret_value;
public:
					CMapEvaluatorConst	(CUICustomMap *object = 0, bool val=false, u16 idx=0, LPCSTR evaluator_name=0):inherited(object,idx,evaluator_name){ret_value=val;};
	virtual bool	evaluate			(){return ret_value;};

};
