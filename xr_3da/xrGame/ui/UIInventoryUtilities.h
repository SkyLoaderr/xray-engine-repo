// CUIInventoryUtilities.h:  функции утилиты для работы с
// различными окнами инвентаря, торговли и т.д.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../inventory_item.h"
#include "UIStatic.h"
#include "../character_info_defs.h"

//////////////////////////////////////////////////////////////////////////

//размеры сетки в текстуре инвентаря
#define INV_GRID_WIDTH			50
#define INV_GRID_HEIGHT			50

//размеры сетки в текстуре иконок персонажей
#define ICON_GRID_WIDTH			64
#define ICON_GRID_HEIGHT		64
//размер иконки персонажа для инвенторя и торговли
#define CHAR_ICON_WIDTH			2
#define CHAR_ICON_HEIGHT		2	

//размер иконки персонажа в полный рост
#define CHAR_ICON_FULL_WIDTH	2
#define CHAR_ICON_FULL_HEIGHT	8	

#define TRADE_ICONS_SCALE		(4.f/5.f)

class CUIDragDropItem;
class CUIWpnDragDropItem;

DEFINE_VECTOR(CUIWpnDragDropItem*, DD_ITEMS_VECTOR, DD_ITEMS_VECTOR_IT);


namespace InventoryUtilities
{

//сравнивает элементы по пространству занимаемому ими в рюкзаке
//для сортировки
bool GreaterRoomInRuck(PIItem item1, PIItem item2);
//для проверки свободного места
bool FreeRoom(TIItemList item_list, int width, int height);

//для надписей на иконках с оружием
void AmmoUpdateProc(CUIDragDropItem* pItem);
//для надписей на иконках с едой
void FoodUpdateProc(CUIDragDropItem* pItem);


//получить shader на иконки инвенторя
ref_shader& GetEquipmentIconsShader();
//shader на иконки персонажей
ref_shader& GetCharIconsShader();
//shader на иконки карты
ref_shader& GetMapIconsShader();
// shader на иконки персонажей в мультиплеере
ref_shader&	GetMPCharIconsShader();
//удаляем все шейдеры
void DestroyShaders();

void ClearDragDrop (DD_ITEMS_VECTOR& dd_item_vector);

// Получить значение времени в текстовом виде

// Точность возвращаемого функцией GetGameDateTimeAsString значения: до часов, до минут, до секунд
enum ETimePrecision
{
	etpTimeToHours = 0,
	etpTimeToMinutes,
	etpTimeToSeconds,
	etpTimeToMilisecs
};

// Точность возвращаемого функцией GetGameDateTimeAsString значения: до года, до месяца, до дня
enum EDatePrecision
{
	edpDateToDay,
	edpDateToMonth,
	edpDateToYear
};

const shared_str GetGameDateAsString(EDatePrecision datePrec, char dateSeparator = '/');
const shared_str GetGameTimeAsString(ETimePrecision timePrec, char timeSeparator = ':');
const shared_str GetDateAsString(ALife::_TIME_ID time, EDatePrecision datePrec, char dateSeparator = '/');
const shared_str GetTimeAsString(ALife::_TIME_ID time, ETimePrecision timePrec, char timeSeparator = ':');

// Отобразить вес, который несет актер
void UpdateWeight(CUIStatic &wnd, bool withPrefix = false);

// Функции получения строки-идентификатора ранга и отношения по их числовому идентификатору
LPCSTR GetRankAsText		(CHARACTER_RANK rankID);
LPCSTR GetReputationAsText	(CHARACTER_REPUTATION rankID);

// Internal namespace for some necessary helper functions
namespace Private
{
	typedef					std::pair<CHARACTER_RANK, shared_str>	CharInfoStringID;
	DEF_MAP					(CharInfoStrings, CHARACTER_RANK, shared_str);
	extern CharInfoStrings	*charInfoReputationStrings;
	extern CharInfoStrings	*charInfoRankStrings;

	void					InitCharacterInfoStrings	();
	void					ClearCharacterInfoStrings	();
	void					LoadStrings					(CharInfoStrings *container, LPCSTR section, LPCSTR field);
}

};