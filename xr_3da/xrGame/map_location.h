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
		level_num = 0;
		x = y = 0;
		name.SetText("");
		text.SetText("");

		attached_to_object = false;
		object_id = 0xffff;
	}

	//����� ������
	int level_num;
	//���������� �� ������
	float x;
	float y;
	//��� �������
	CUIString name;
	//����� ��������
	CUIString text;

	//������������ �� ������� � �������
	bool attached_to_object;
	//id ������� �� ������
	u16 object_id;
	//������ ����� ����������
	INFO_ID info_portion_id;

	//������� � ��������� ������
	int icon_x, icon_y, icon_width, icon_height;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);