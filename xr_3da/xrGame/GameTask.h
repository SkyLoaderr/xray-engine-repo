///////////////////////////////////////////////////////////////
// GameTask.h
// Класс игрового задания задания
///////////////////////////////////////////////////////////////

#pragma once
/*
//#include "ui/xrXMLParser.h"

//#include "shared_data.h"
//#include "xml_str_id_loader.h"

//#include "encyclopedia_article_defs.h"
//#include "GameTaskDefs.h"

class  CGameObject;


//подцель задания 
struct SGameTaskObjective 
{
	SGameTaskObjective() {}
	//текстовое описание
	shared_str description;
	//index статьи в энциклопедии или дневнике (если есть)
	ARTICLE_ID	article_id;
	shared_str	map_location;
	u16			object_id;

	//прикрипленная иконка
	shared_str icon_texture_name;
	float icon_x, icon_y, icon_width, icon_height;
};

//////////////////////////////////////////////////////////////////////////
// SPhraseDialogData: данные для представления диалога
//////////////////////////////////////////////////////////////////////////
struct SGameTaskData : CSharedResource
{
	SGameTaskData ();
	virtual ~SGameTaskData ();

	DEFINE_VECTOR(SGameTaskObjective, OBJECTIVE_VECTOR, OBJECTIVE_VECTOR_IT);
	OBJECTIVE_VECTOR m_Objectives;
	//название задания
	shared_str title;
};


class CGameTask: public CSharedClass<SGameTaskData, TASK_ID, false>,
				 public CXML_IdToIndex<TASK_ID, int, CGameTask>
{
private:
	typedef CSharedClass<SGameTaskData, TASK_ID, false>	inherited_shared;
	typedef CXML_IdToIndex<TASK_ID, int, CGameTask>	id_to_index;

	friend id_to_index;
public:
			CGameTask	();
	virtual ~CGameTask	();

	virtual void Load	(TASK_ID  id);

	virtual void Init	(const TASK_DATA&  data);

protected:
	TASK_ID					m_TaskId;
	void					load_shared			(LPCSTR);
	static void				InitXmlIdToIndex	();
public:
	const TASK_ID			Id					() {return m_TaskId;}
	SGameTaskData*			data				() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

public:
			u32				ObjectivesNum		();
			shared_str		ObjectiveDesc		(u32 index);
			shared_str		ObjectiveTitle		() {return data()->title;}
			ARTICLE_ID		ObjectiveArticle	(u32 index);
			ETaskState		ObjectiveState		(u32 index);
			Flags16&		ObjectiveFlag		(u32 index);

			void			ObjectiveIcon	(	u32 index, shared_str& tex_name, float& x, float& y, float& width, float& height);
	//инициализируется значениями из реестра актера
	TASK_STATE_VECTOR		m_ObjectiveStates;
	ALife::_TIME_ID			m_ReceiveTime;
	ALife::_TIME_ID			m_FinishTime;
};
*/
//---------------------------------------------------------------------------------

#include "encyclopedia_article_defs.h"
#include "GameTaskDefs.h"

class CGameTaskManager;

struct SGameTaskObjective 
{
	SGameTaskObjective		()				{}
	shared_str				description;
	ARTICLE_ID				article_id;
	shared_str				map_location;
	u16						object_id;
	ETaskState				task_state;
	Flags16					flags;

	//прикрипленная иконка
	shared_str				icon_texture_name;
	Frect					icon_rect;//x,y,w,h
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

	bool					Highlighted				(int objective_id);						// onMap
	void					Highlight				(int objective_id, bool bHighlight);	// onMap
	//название задания
	shared_str				m_Title;
	OBJECTIVE_VECTOR		m_Objectives;
	ALife::_TIME_ID			m_ReceiveTime;
	ALife::_TIME_ID			m_FinishTime;
	
};