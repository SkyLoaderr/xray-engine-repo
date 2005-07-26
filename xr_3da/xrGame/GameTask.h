///////////////////////////////////////////////////////////////
// GameTask.h
// ����� �������� ������� �������
///////////////////////////////////////////////////////////////

#pragma once

#include "encyclopedia_article_defs.h"
#include "GameTaskDefs.h"

class CGameTaskManager;
class CMapLocation;

struct SGameTaskObjective 
{
	SGameTaskObjective		():description(NULL),article_id(NULL),map_location(NULL),object_id(u16(-1))	{}
	shared_str				description;
	ARTICLE_ID				article_id;
	shared_str				map_location;
	u16						object_id;
	ETaskState				task_state;
	CMapLocation*			HasMapLocation		();
	

	//������������� ������
	shared_str				icon_texture_name;
	Frect					icon_rect;//x,y,w,h
	bool					def_location_enabled;
};
DEFINE_VECTOR(SGameTaskObjective, OBJECTIVE_VECTOR, OBJECTIVE_VECTOR_IT);

class CGameTask
{
private:
							CGameTask				(const CGameTask&){}; //disable copy ctor
protected:
	void					Load					(const TASK_ID& id);
public:
							CGameTask				(const TASK_ID& id);

	bool					HighlightedSpotOnMap	(int objective_id);					
	void					HighlightSpotOnMap		(int objective_id, bool bHighlight);
	
	bool					HasMapLocations			();
	void					ShowLocations			(bool bShow);
	bool					ShownLocations			();
	
	shared_str				m_ID;
	shared_str				m_Title;
	OBJECTIVE_VECTOR		m_Objectives;
	ALife::_TIME_ID			m_ReceiveTime;
	ALife::_TIME_ID			m_FinishTime;
	
};