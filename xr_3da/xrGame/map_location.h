///////////////////////////////////////////////////////////////
// map_location.h
// структура дл€ локации на карте
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/uistring.h"
#include "infoportiondefs.h"

//-----------------------------------------------------------------------------/
//  Forward declarations
//-----------------------------------------------------------------------------/

class CLAItem;

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

	//им€ уровн€
	shared_str level_name;
	//координаты на уровне
	float x;
	float y;
	//им€ локации
	shared_str name;
	//текст описани€
	shared_str text;

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

	// ѕоказывать или не показывать отметку этого маплокейшина на карте
	void	UpdateAnimation();
	// «адать цветовую анимацию дл€ спота
	void	SetColorAnimation(const shared_str &animationName);

private:
	// јнимаци€ индикатора на карте
	CLAItem	*colorAnimation;
	// “екущиее врем€ проигрывани€ анимации
	float	animationTime;
	float	prevTimeGlobal;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);