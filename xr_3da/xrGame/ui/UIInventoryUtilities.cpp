// CUIInventoryUtilities.cpp:  функции утилиты для работы с
// различными окнами инвентаря 
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "UIInventoryUtilities.h"
#include "../WeaponAmmo.h"
#include "../UIStaticItem.h"
#include "UIDragDropItem.h"
#include "UIStatic.h"
#include "../eatable_item.h"
#include "../Level.h"
#include "../HUDManager.h"

//буферный список для сортировки
static ref_shader g_EquipmentIconsShader	= NULL;
static ref_shader g_CharIconsShader			= NULL;
static ref_shader g_MPCharIconsShader		= NULL;
static CUIStatic*	GetUIStatic();



//для надписей на иконках с оружием
void InventoryUtilities::AmmoUpdateProc(CUIDragDropItem* pItem)
{
	CInventoryItem* pIItem = (CInventoryItem*)(pItem->GetData());
	CWeaponAmmo* pAmmoItem = dynamic_cast<CWeaponAmmo*>(pIItem);

	int left	= pItem->GetUIStaticItem().GetPosX() + 5;
	int bottom	= pItem->GetUIStaticItem().GetPosY() + pItem->GetUIStaticItem().GetRect().height();
	
	HUD().OutText(pItem->GetFont(), pItem->GetClipRect(), float(left), 
						float(bottom - pItem->GetFont()->CurrentHeight()),
						"%d",	pAmmoItem->m_boxCurr);
	pItem->GetFont()->OnRender();
}

//для надписей на иконках с едой
void InventoryUtilities::FoodUpdateProc(CUIDragDropItem* pItem)
{
	//CEatableItem* pEatableItem = (CEatableItem*)(pItem->GetData());
	CInventoryItem* pIItem = (CInventoryItem*)(pItem->GetData()); R_ASSERT(pIItem);
	CEatableItem* pEatableItem = dynamic_cast<CEatableItem*>(pIItem); R_ASSERT(pEatableItem);

	RECT rect = pItem->GetAbsoluteRect();
	
	if(pEatableItem->m_iPortionsNum>0)
		HUD().OutText(pItem->GetFont(), pItem->GetClipRect(), float(rect.left), 
							float(rect.bottom - pItem->GetFont()->CurrentHeight()- 2),
							"%d",	pEatableItem->m_iPortionsNum);
	pItem->GetFont()->OnRender();
}


//сравнивает элементы по пространству занимаемому ими в рюкзаке
bool InventoryUtilities::GreaterRoomInRuck(PIItem item1, PIItem item2)
{
	int item1_room = item1->GetGridWidth()*item1->GetGridHeight();
	int item2_room = item2->GetGridWidth()*item2->GetGridHeight();

	if(item1_room > item2_room)
		return true;
	else if (item1_room == item2_room)
	{
		if(item1->GetGridWidth() >= item2->GetGridWidth())
			return true;
	}
   	return false;
}

//static bool ruck_room[1000];

bool InventoryUtilities::FreeRoom(TIItemList item_list, int width, int height)
{
	//bool* ruck_room = (BOOL*)xr_malloc(width*height*sizeof(BOOL));
	xr_vector<bool> ruck_room;
	ruck_room.resize(width*height);

	int i,j,k,m;
	int place_row = 0,  place_col = 0;
	bool found_place;
	bool can_place;


	for(i=0; i<height; ++i)
		for(j=0; j<width; ++j)
			ruck_room[i*width + j] = false;


	std::vector<PIItem> ruck_list;
	ruck_list.clear();
	ruck_list.insert(ruck_list.begin(),
					 item_list.begin(),
					 item_list.end());
					
	//ruck_list = item_list;
	
	std::sort(ruck_list.begin(), ruck_list.end(),GreaterRoomInRuck);
	
	found_place = true;

	for(std::vector<PIItem>::iterator it = ruck_list.begin(); 
						(ruck_list.end() != it) && found_place; ++it) 
	{
		PIItem pItem = *it;

		//проверить можно ли разместить элемент,
		//проверяем последовательно каждую клеточку
		found_place = false;
	
		for(i=0; (i<height - pItem->GetGridHeight() +1) && !found_place; ++i)
		{
			for(j=0; (j<width - pItem->GetGridWidth() +1) && !found_place; ++j)
			{
				can_place = true;

				for(k=0; (k<pItem->GetGridHeight()) && can_place; ++k)
				{
					for(m=0; (m<pItem->GetGridWidth()) && can_place; ++m)
					{
						if(ruck_room[(i+k)*width + (j+m)])
								can_place =  false;
					}
				}
			
				if(can_place)
				{
					found_place=true;
					place_row = i;
					place_col = j;
				}

			}
		}

		//разместить элемент на найденном месте
		if(found_place)
		{
			for(k=0; k<pItem->GetGridHeight(); ++k)
			{
				for(m=0; m<pItem->GetGridWidth(); ++m)
				{
					ruck_room[(place_row+k)*width + place_col+m] = true;
				}
			}
		}
	}

	//для какого-то элемента места не нашлось
	if(!found_place) return false;

	return true;
}

ref_shader& InventoryUtilities::GetEquipmentIconsShader()
{	
	if(!g_EquipmentIconsShader)
	{
		g_EquipmentIconsShader.create("hud\\default", 
									  "ui\\ui_icon_equipment");
	}

	return g_EquipmentIconsShader;
}

ref_shader& InventoryUtilities::GetCharIconsShader()
{
	if(!g_CharIconsShader)
	{
		g_CharIconsShader.create("hud\\default",  "ui\\ui_icons_npc");
	}

	return g_CharIconsShader;
}

////////////////////////////////////////////////////////////////////////////////

// shader на иконки персонажей в мультиплеере
ref_shader&	InventoryUtilities::GetMPCharIconsShader()
{
	if(!g_MPCharIconsShader)
	{
		g_MPCharIconsShader.create("hud\\default",  "ui\\ui_models_multiplayer");
	}

	return g_MPCharIconsShader;
}



void InventoryUtilities::ClearDragDrop (DD_ITEMS_VECTOR& dd_item_vector)
{
	for(u32 i=0; i<dd_item_vector.size(); i++)
		xr_delete(dd_item_vector[i]);
	dd_item_vector.clear();
}

//////////////////////////////////////////////////////////////////////////

const ref_str InventoryUtilities::GetGameTimeAsString(EGetTimePrecision precision, char separator)
{
	string32 buf;
	ALife::_TIME_ID currMsec = Level().GetGameTime();

	u8 currSecs		= static_cast<u8>(currMsec / 1000 % 60 & 0xFF);
	u8 currMins		= static_cast<u8>(currMsec / (1000 * 60) % 60 & 0xFF);
	u8 currHours	= static_cast<u8>(currMsec / (1000 * 3600) % 24 & 0xFF);

	switch (precision)
	{
	case egtpToHours:
		sprintf(buf, "%02i", currHours);
		break;
	case egtpToMinutes:
		sprintf(buf, "%02i%c%02i", currHours, separator, currMins);
		break;
	case egtpToSeconds:
		sprintf(buf, "%02i%c%02i%c%02i", currHours, separator, currMins, separator, currSecs);
		break;
	default:
		R_ASSERT(!"Unknown type of precision");
	}

	return buf;
}
