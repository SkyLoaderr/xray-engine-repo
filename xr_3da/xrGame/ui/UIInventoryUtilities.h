// CUIInventoryUtilities.h:  функции утилиты для работы с
// различными окнами инвентаря, торговли и т.д.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../inventory_item.h"
#include "../UIStaticItem.h"

//размеры сетки в текстуре инвентаря
#define INV_GRID_WIDTH		50
#define INV_GRID_HEIGHT		50

//размеры сетки в текстуре иконок персонажей
#define ICON_GRID_WIDTH		64
#define ICON_GRID_HEIGHT	64
//размер иконки персонажа для инвенторя и торговли
#define CHAR_ICON_WIDTH		2
#define CHAR_ICON_HEIGHT	2	

//размер иконки персонажа в полный рост
#define CHAR_ICON_FULL_WIDTH	2
#define CHAR_ICON_FULL_HEIGHT	8	




class CUIDragDropItem;


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
// shader на иконки персонажей в мультиплеере
ref_shader&	GetMPCharIconsShader();

};