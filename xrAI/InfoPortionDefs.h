///////////////////////////////////////////////////////////////
// InfoPortionsDefs.h
// ����� ���������� ��� ������� ���������� � info_portion
///////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

//#define NO_INFO_INDEX	(-1)
//typedef int				INFO_INDEX;
typedef shared_str		INFO_ID;


struct INFO_DATA
{
	INFO_DATA():info_id(NULL),receive_time(0){};
	INFO_DATA(INFO_ID id, ALife::_TIME_ID time):info_id(id),receive_time(time){};

	INFO_ID			info_id;
	//����� ��������� ����� ������ ����������
	ALife::_TIME_ID		receive_time;
};

DEFINE_VECTOR		(INFO_DATA, KNOWN_INFO_VECTOR, KNOWN_INFO_VECTOR_IT);