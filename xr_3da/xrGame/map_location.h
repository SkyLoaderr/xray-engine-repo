///////////////////////////////////////////////////////////////
// map_location.h
// структура дл€ локации на карте
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
		
		marker_show = false;
		icon_color = 0xffffffff;
	}

	//им€ уровн€
	ref_str level_name;
	//координаты на уровне
	float x;
	float y;
	//им€ локации
	ref_str name;
	//текст описани€
	ref_str text;

	//присоединина ли локаци€ к объекту
	bool attached_to_object;
	//id объекта на уровне
	u16 object_id;
	//индекс части информации
	INFO_INDEX info_portion_id;

	//размеры и положение иконки
	int icon_x, icon_y, icon_width, icon_height;
	
	//нужно ли показывать маркер (стрелку на краю карты)
	bool marker_show;
	//цвет стрелки маркера и иконки на миникарте
	u32 icon_color;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);