///////////////////////////////////////////////////////////////
// map_location.h
// ��������� ��� ������� �� �����
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/uistring.h"
#include "infoportiondefs.h"


struct SMapLocation
{
	SMapLocation() 
	{
		info_portion_id = NO_INFO_INDEX;
		level_name = NULL;
		x = y = 0;
		name = NULL;
		text = NULL;

		attached_to_object = false;
		object_id = 0xffff;
	}

	//��� ������
	ref_str level_name;
	//���������� �� ������
	float x;
	float y;
	//��� �������
	ref_str name;
	//����� ��������
	ref_str text;

	//������������ �� ������� � �������
	bool attached_to_object;
	//id ������� �� ������
	u16 object_id;
	//������ ����� ����������
	INFO_INDEX info_portion_id;

	//������� � ��������� ������
	int icon_x, icon_y, icon_width, icon_height;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);