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
#include "../date_time.h"

//////////////////////////////////////////////////////////////////////////

#define EQUIPMENT_ICONS "ui\\ui_icon_equipment"
#define CHAR_ICONS		"ui\\ui_icons_npc"
#define MAP_ICONS		"ui\\ui_icons_map"
#define MP_CHAR_ICONS	"ui\\ui_models_multiplayer"


static ref_shader g_EquipmentIconsShader	= NULL;
static ref_shader g_CharIconsShader			= NULL;
static ref_shader g_MapIconsShader			= NULL;
static ref_shader g_MPCharIconsShader		= NULL;
static CUIStatic*	GetUIStatic();

//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::DestroyShaders()
{
	g_EquipmentIconsShader.destroy();
	g_CharIconsShader.destroy();	
	g_MapIconsShader.destroy();
	g_MPCharIconsShader.destroy();
}


//для надписей на иконках с оружием
void InventoryUtilities::AmmoUpdateProc(CUIDragDropItem* pItem)
{
	CInventoryItem* pIItem = (CInventoryItem*)(pItem->GetData());
	CWeaponAmmo* pAmmoItem = dynamic_cast<CWeaponAmmo*>(pIItem);

	int left	= pItem->GetUIStaticItem().GetPosX() + 5;
	int bottom	= pItem->GetUIStaticItem().GetPosY() + pItem->GetUIStaticItem().GetRect().height();
	
	pItem->GetFont()->SetColor(0xffffffff);
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
	
	pItem->GetFont()->SetColor(0xffffffff);
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
		g_EquipmentIconsShader.create("hud\\default", EQUIPMENT_ICONS);
	}

	return g_EquipmentIconsShader;
}

ref_shader& InventoryUtilities::GetCharIconsShader()
{
	if(!g_CharIconsShader)
	{
		g_CharIconsShader.create("hud\\default", CHAR_ICONS);
	}

	return g_CharIconsShader;
}

ref_shader&  InventoryUtilities::GetMapIconsShader()
{
	if(!g_MapIconsShader)
	{
		g_MapIconsShader.create("hud\\default",  MAP_ICONS);
	}

	return g_MapIconsShader;
}

////////////////////////////////////////////////////////////////////////////////

// shader на иконки персонажей в мультиплеере
ref_shader&	InventoryUtilities::GetMPCharIconsShader()
{
	if(!g_MPCharIconsShader)
	{
		g_MPCharIconsShader.create("hud\\default",  MP_CHAR_ICONS);
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

const ref_str InventoryUtilities::GetGameDateAsString(EDatePrecision datePrec, char dateSeparator)
{
	return GetDateAsString(Level().GetGameTime(), datePrec, dateSeparator);
}

const ref_str InventoryUtilities::GetGameTimeAsString(ETimePrecision timePrec, char timeSeparator)
{
	return GetTimeAsString(Level().GetGameTime(), timePrec, timeSeparator);
}

//////////////////////////////////////////////////////////////////////////

const ref_str InventoryUtilities::GetTimeAsString(ALife::_TIME_ID time, ETimePrecision timePrec, char timeSeparator)
{
	string32 bufTime;

	ZeroMemory(bufTime, 32);

	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;

	split_time(time, year, month, day, hours, mins, secs, milisecs);

	// Time
	switch (timePrec)
	{
	case etpTimeToHours:
		sprintf(bufTime, "%02i", hours);
		break;
	case etpTimeToMinutes:
		sprintf(bufTime, "%02i%c%02i", hours, timeSeparator, mins);
		break;
	case etpTimeToSeconds:
		sprintf(bufTime, "%02i%c%02i%c%02i", hours, timeSeparator, mins, timeSeparator, secs);
		break;
	case etpTimeToMilisecs:
		sprintf(bufTime, "%02i%c%02i%c%02i%c%02i", hours, timeSeparator, mins, timeSeparator, secs, timeSeparator, milisecs);
		break;
	default:
		R_ASSERT(!"Unknown type of date precision");
	}

	return bufTime;
}

//////////////////////////////////////////////////////////////////////////

const ref_str InventoryUtilities::GetDateAsString(ALife::_TIME_ID date, EDatePrecision datePrec, char dateSeparator)
{
	string32 bufDate;

	ZeroMemory(bufDate, 32);

	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;

	split_time(date, year, month, day, hours, mins, secs, milisecs);

	// Date
	switch (datePrec)
	{
	case edpDateToYear:
		sprintf(bufDate, "%04i", year);
		break;
	case edpDateToMonth:
		sprintf(bufDate, "%02i%c%04i", month, dateSeparator, year);
		break;
	case edpDateToDay:
		sprintf(bufDate, "%02i%c%02i%c%04i", day, dateSeparator, month, dateSeparator, year);
		break;
	default:
		R_ASSERT(!"Unknown type of date precision");
	}

	return bufDate;
}