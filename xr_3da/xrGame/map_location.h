///////////////////////////////////////////////////////////////
// map_location.h
// структура для локации на карте
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/uistring.h"
#include "infoportiondefs.h"
#include "ui/UIColorAnimatorWrapper.h"

//////////////////////////////////////////////////////////////////////////

enum EMapLocationFlags
{
	eMapLocationPDAContact		= 0x01,
	eMapLocationInfoPortion		= 0x02,
	eMapLocationScript			= 0x04
};

//////////////////////////////////////////////////////////////////////////

struct SMapLocation
{
	SMapLocation();

	shared_str LevelName();

	//типы локаций
	flags32 type_flags;

	//имя уровня
	shared_str level_name;
	//координаты на уровне
	float x;
	float y;
	//имя локации
	shared_str name;
	//текст описания
	shared_str text;

	//присоединина ли локация к объекту
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

	// Показывать или не показывать отметку этого маплокейшина на карте
	void	UpdateAnimation();
	// Задать цветовую анимацию для спота
	void	SetColorAnimation(const shared_str &animationName);

	// Динамическое проявление и исчезание
	bool	dynamic_manifestation;

private:
	// Анимация индикатора на карте
	CUIColorAnimatorWrapper	animation;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);