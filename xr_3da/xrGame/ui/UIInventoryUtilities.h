// CUIInventoryUtilities.h:  функции утилиты для работы с
// различными окнами инвентаря, торговли и т.д.
//////////////////////////////////////////////////////////////////////



#include "..\inventory.h"
#include "UIDragDropItem.h"
#include "..\\UIStaticItem.h"


#define INV_GRID_WIDTH	50
#define INV_GRID_HEIGHT	50

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


};