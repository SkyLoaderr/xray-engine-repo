///////////////////////////////////////////////////////////////
// map_location.h
// структура дл€ локации на карте
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
	SMapLocation(const SMapLocation&);

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
	// set this field to texture name you want to use instead of default
	shared_str shader_name;	
	
	//нужно ли показывать маркер (стрелку на краю карты)
	bool marker_show;
	//показывать специальную большую иконку вместо точки (сюжетную)
	bool big_icon;
	//цвет стрелки маркера и иконки на миникарте
	u32 icon_color;
	//показывать ли на глобальной карте им€ локации (когда наводишь на карту)
	bool global_map_text;

	// ѕоказывать или не показывать отметку этого маплокейшина на карте
	void	UpdateAnimation();
	// «адать цветовую анимацию дл€ спота
	void	SetColorAnimation(const shared_str &animationName);

	ref_shader&	GetShader();

	// ƒинамическое про€вление и исчезание
	bool	dynamic_manifestation;

private:
	// јнимаци€ индикатора на карте
	CUIColorAnimatorWrapper	animation;
	ref_shader	m_iconsShader;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);