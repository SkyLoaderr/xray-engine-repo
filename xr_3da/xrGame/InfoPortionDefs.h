///////////////////////////////////////////////////////////////
// InfoPortionsDefs.h
// общие объ€влени€ дл€ классов работающих с info_portion
///////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

#define NO_INFO_INDEX	(-1)
typedef int				INFO_INDEX;
typedef shared_str		INFO_STR_ID;


struct INFO_DATA
{
	INFO_DATA():id(NO_INFO_INDEX),receive_time(0){};
	INFO_DATA(INFO_INDEX info_index, ALife::_TIME_ID time):id(info_index),receive_time(time){};

	INFO_INDEX			id;
	//врем€ получени€ нужно порции информации
	ALife::_TIME_ID		receive_time;
};

DEFINE_VECTOR		(INFO_DATA, KNOWN_INFO_VECTOR, KNOWN_INFO_VECTOR_IT);