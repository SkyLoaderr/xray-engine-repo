///////////////////////////////////////////////////////////////
// GameTaksDefs.h
// Класс игрового задания задания
///////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "object_interfaces.h"

//typedef int			TASK_INDEX;
typedef shared_str		TASK_ID;
//#define NO_TASK		TASK_INDEX(-1)


//состояние цели и задания
enum ETaskState {
	eTaskStateFail			= 0,
	eTaskStateInProgress	= 1,
	eTaskStateCompleted		= 2,
	eTaskStateDummy			= u32(-1)
};


DEFINE_VECTOR		(ETaskState, TASK_STATE_VECTOR, TASK_STATE_IT);


struct TASK_DATA : public IPureSerializeObject<IReader,IWriter>
{
	TASK_DATA();
	TASK_DATA(TASK_ID id, ALife::_TIME_ID time);

	virtual void load (IReader&);
	virtual void save (IWriter&);

	TASK_ID			task_id;
	//время получения задания
	ALife::_TIME_ID		receive_time;
	//время окончания задания
	ALife::_TIME_ID		finish_time;
	//текущее состояние задания и подзаданий
	TASK_STATE_VECTOR	states;
};

DEFINE_VECTOR		(TASK_DATA, GAME_TASK_VECTOR, GAME_TASK_IT);
DEFINE_VECTOR		(TASK_ID, TASK_ID_VECTOR, TASK_ID_IT);