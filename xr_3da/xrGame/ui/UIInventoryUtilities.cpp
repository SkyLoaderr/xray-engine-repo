// UIInventoryUtilities.cpp:  функции утилиты для работы с
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
#include "../string_table.h"
#include "../Inventory.h"
#include "../InventoryOwner.h"

#include "../InfoPortion.h"
#include "../game_base_space.h"
#include "../actor.h"
#include "uiwpndragdropitem.h"

#define BUY_MENU_TEXTURE "ui\\ui_mp_buy_menu"
#define EQUIPMENT_ICONS  "ui\\ui_icon_equipment"
#define CHAR_ICONS		 "ui\\ui_icons_npc"
#define MAP_ICONS		 "ui\\ui_icons_map"
#define MP_CHAR_ICONS	 "ui\\ui_models_multiplayer"

const LPCSTR relationsLtxSection	= "game_relations";
const LPCSTR ratingField			= "rating_names";
const LPCSTR reputationgField		= "reputation_names";
const LPCSTR goodwillField		= "goodwill_names";

static ref_shader	g_BuyMenuShader			= NULL;
static ref_shader	g_EquipmentIconsShader	= NULL;
static ref_shader	g_CharIconsShader		= NULL;
static ref_shader	g_MapIconsShader		= NULL;
static ref_shader	g_MPCharIconsShader		= NULL;
static CUIStatic*	GetUIStatic				();
const int			hugeValue				= 0x0ddddddd;

typedef				std::pair<CHARACTER_RANK_VALUE, shared_str>	CharInfoStringID;
DEF_MAP				(CharInfoStrings, CHARACTER_RANK_VALUE, shared_str);

CharInfoStrings		*charInfoReputationStrings	= NULL;
CharInfoStrings		*charInfoRankStrings		= NULL;
CharInfoStrings		*charInfoGoodwillStrings	= NULL;

//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::DestroyShaders()
{
	g_BuyMenuShader.destroy();
	g_EquipmentIconsShader.destroy();
	g_CharIconsShader.destroy();	
	g_MapIconsShader.destroy();
	g_MPCharIconsShader.destroy();
}

//////////////////////////////////////////////////////////////////////////
//для надписей на иконках с оружием
//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::AmmoDrawProc(CUIDragDropItem* pItem)
{
	CInventoryItem* pIItem = (CInventoryItem*)(pItem->GetData());
	CWeaponAmmo* pAmmoItem = smart_cast<CWeaponAmmo*>(pIItem);

	if (pAmmoItem->m_boxCurr > 1)
	{
		float left	= pItem->GetUIStaticItem().GetPosX() + 5;
		float bottom	= pItem->GetUIStaticItem().GetPosY() + pItem->GetUIStaticItem().GetRect().height();

		pItem->GetFont()->SetColor(0xffffffff);
		UI()->OutText(	pItem->GetFont(), pItem->GetSelfClipRect(), left, 
						bottom - pItem->GetFont()->CurrentHeight(),
						"%d",	pAmmoItem->m_boxCurr);

		pItem->GetFont()->OnRender();
	}
}

//////////////////////////////////////////////////////////////////////////
//для надписей на иконках с едой
//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::FoodDrawProc(CUIDragDropItem* pItem)
{
	//CEatableItem* pEatableItem = (CEatableItem*)(pItem->GetData());
	CInventoryItem* pIItem = (CInventoryItem*)(pItem->GetData()); R_ASSERT(pIItem);
	CEatableItem* pEatableItem = smart_cast<CEatableItem*>(pIItem); R_ASSERT(pEatableItem);

	if (pEatableItem->PortionsNum() > 1)
	{
		Frect rect = pItem->GetAbsoluteRect();

		pItem->GetFont()->SetColor(0xffffffff);
		UI()->OutText(	pItem->GetFont(), 
						pItem->GetSelfClipRect(), 
						rect.left, 
						float(rect.bottom - pItem->GetFont()->CurrentHeight()- 2),
						"%d",	pEatableItem->PortionsNum());

		pItem->GetFont()->OnRender();
	}
}


//////////////////////////////////////////////////////////////////////////
//сравнивает элементы по пространству занимаемому ими в рюкзаке
//////////////////////////////////////////////////////////////////////////

bool InventoryUtilities::GreaterRoomInRuck(PIItem item1, PIItem item2)
{
	Ivector2 r1,r2;
	r1.x			= item1->GetGridWidth();
	r1.y			= item1->GetGridHeight();
	r2.x			= item2->GetGridWidth();
	r2.y			= item2->GetGridHeight();

	if(r1.x > r2.x)			return true;
	
	if (r1.x == r2.x)
	{
		if(r1.y > r2.y)		return true;
		
		if (r1.y == r2.y)	return (item1->object().ID() > item2->object().ID());

		return				false;
	}

   	return					false;
}

//////////////////////////////////////////////////////////////////////////
bool InventoryUtilities::FreeRoom_inBelt	(TIItemContainer& item_list, PIItem _item, int width, int height)
{
	bool*				ruck_room	= (bool*)alloca(width*height);

	int		i,j,k,m;
	int		place_row = 0,  place_col = 0;
	bool	found_place;
	bool	can_place;


	for(i=0; i<height; ++i)
		for(j=0; j<width; ++j)
			ruck_room[i*width + j]	= false;

	item_list.push_back	(_item);
	std::sort			(item_list.begin(), item_list.end(),GreaterRoomInRuck);
	
	found_place			= true;

	for(xr_vector<PIItem>::iterator it = item_list.begin(); (item_list.end() != it) && found_place; ++it) 
	{
		PIItem pItem = *it;
		int iWidth	= pItem->GetGridWidth(); 
		int iHeight = pItem->GetGridHeight();
		//проверить можно ли разместить элемент,
		//проверяем последовательно каждую клеточку
		found_place = false;
	
		for(i=0; (i<height - iHeight +1) && !found_place; ++i)
		{
			for(j=0; (j<width - iWidth +1) && !found_place; ++j)
			{
				can_place = true;

				for(k=0; (k<iHeight) && can_place; ++k)
				{
					for(m=0; (m<iHeight) && can_place; ++m)
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
			for(k=0; k<iHeight; ++k)
			{
				for(m=0; m<iWidth; ++m)
				{
					ruck_room[(place_row+k)*width + place_col+m] = true;
				}
			}
		}
	}

	// remove
	item_list.erase	(std::remove(item_list.begin(),item_list.end(),_item),item_list.end());

	//для какого-то элемента места не нашлось
	if(!found_place) return false;

	return true;
}

/////////////////////////////////////////////////////////////

ref_shader& InventoryUtilities::GetBuyMenuShader()
{	
	if(!g_BuyMenuShader)
	{
		g_BuyMenuShader.create("hud\\default", BUY_MENU_TEXTURE);
	}

	return g_BuyMenuShader;
}
//////////////////////////////////////////////////////////////////////////

ref_shader& InventoryUtilities::GetEquipmentIconsShader()
{	
	if(!g_EquipmentIconsShader)
	{
		g_EquipmentIconsShader.create("hud\\default", EQUIPMENT_ICONS);
	}

	return g_EquipmentIconsShader;
}

//////////////////////////////////////////////////////////////////////////

ref_shader& InventoryUtilities::GetCharIconsShader()
{
	if(!g_CharIconsShader)
	{
		g_CharIconsShader.create("hud\\default", CHAR_ICONS);
	}

	return g_CharIconsShader;
}

//////////////////////////////////////////////////////////////////////////

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
//////////////////////////////////////////////////////////////////////////

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

const shared_str InventoryUtilities::GetGameDateAsString(EDatePrecision datePrec, char dateSeparator)
{
	return GetDateAsString(Level().GetGameTime(), datePrec, dateSeparator);
}

//////////////////////////////////////////////////////////////////////////

const shared_str InventoryUtilities::GetGameTimeAsString(ETimePrecision timePrec, char timeSeparator)
{
	return GetTimeAsString(Level().GetGameTime(), timePrec, timeSeparator);
}

//////////////////////////////////////////////////////////////////////////

const shared_str InventoryUtilities::GetTimeAsString(ALife::_TIME_ID time, ETimePrecision timePrec, char timeSeparator)
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

const shared_str InventoryUtilities::GetDateAsString(ALife::_TIME_ID date, EDatePrecision datePrec, char dateSeparator)
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

//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::UpdateWeight(CUIStatic &wnd, bool withPrefix)
{
	CInventoryOwner *pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	R_ASSERT(pInvOwner);
	string128 buf;
	ZeroMemory(buf, 128);

	float total = pInvOwner->inventory().CalcTotalWeight();
	float max	= pInvOwner->MaxCarryWeight();

	string16 cl;
	ZeroMemory(cl, 16);

	if (total > max)
	{
		strcpy(cl, "%c<red>");
	}
	else
	{
		strcpy(cl, "%c<UI_orange>");
	}

	string32 prefix;
	ZeroMemory(prefix, 32);

	if (withPrefix)
	{
		sprintf(prefix, "%%c<default>%s ", *CStringTable().translate("ui_inv_weight"));
	}
	else
	{
		strcpy(prefix, "");
	}

	sprintf(buf, "%s%s%3.1f %s/%5.1f", prefix, cl, total, "%c<UI_orange>", max);
	wnd.SetText(buf);
	//	UIStaticWeight.ClipperOff();
}

//////////////////////////////////////////////////////////////////////////

void LoadStrings(CharInfoStrings *container, LPCSTR section, LPCSTR field)
{
	R_ASSERT(container);

	LPCSTR				cfgRecord	= pSettings->r_string(section, field);
	u32					count		= _GetItemCount(cfgRecord);
	R_ASSERT3			(count%2, "there're must be an odd number of elements", field);
	string32			singleThreshold;
	ZeroMemory			(singleThreshold, sizeof(singleThreshold));
	int					upBoundThreshold	= 0;
	CharInfoStringID	id;

	for (u32 k = 0; k < count; k += 2)
	{
		_GetItem(cfgRecord, k, singleThreshold);
		id.second = singleThreshold;

		if (count == k + 1)
		{
			// Indicate greatest value
			id.first = hugeValue;
		}
		else
		{
			_GetItem(cfgRecord, k + 1, singleThreshold);
			sscanf(singleThreshold, "%i", &upBoundThreshold);
			id.first = upBoundThreshold;
		}
		container->insert(id);
	}
}

//////////////////////////////////////////////////////////////////////////

void InitCharacterInfoStrings()
{
	if (charInfoReputationStrings && charInfoRankStrings) return;

	if (!charInfoReputationStrings)
	{
		// Create string->Id DB
		charInfoReputationStrings	= xr_new<CharInfoStrings>();
		// Reputation
		LoadStrings(charInfoReputationStrings, relationsLtxSection, reputationgField);
	}

	if (!charInfoRankStrings)
	{
		// Create string->Id DB
		charInfoRankStrings			= xr_new<CharInfoStrings>();
		// Ranks
		LoadStrings(charInfoRankStrings, relationsLtxSection, ratingField);
	}

	if (!charInfoGoodwillStrings)
	{
		// Create string->Id DB
		charInfoGoodwillStrings			= xr_new<CharInfoStrings>();
		// Goodwills
		LoadStrings(charInfoGoodwillStrings, relationsLtxSection, goodwillField);
	}

}

//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::ClearCharacterInfoStrings()
{
	xr_delete(charInfoReputationStrings);
	xr_delete(charInfoRankStrings);
	xr_delete(charInfoGoodwillStrings);
}

//////////////////////////////////////////////////////////////////////////

LPCSTR InventoryUtilities::GetRankAsText(CHARACTER_RANK_VALUE rankID)
{
	InitCharacterInfoStrings();
	R_ASSERT(charInfoRankStrings);

	CharInfoStrings::const_iterator cit = charInfoRankStrings->upper_bound(rankID);
	R_ASSERT(charInfoRankStrings->end() != cit);
	return *cit->second;
}

//////////////////////////////////////////////////////////////////////////

LPCSTR InventoryUtilities::GetReputationAsText(CHARACTER_REPUTATION_VALUE rankID)
{
	InitCharacterInfoStrings();
	R_ASSERT(charInfoReputationStrings);

	CharInfoStrings::const_iterator cit = charInfoReputationStrings->upper_bound(rankID);
	R_ASSERT(charInfoReputationStrings->end() != cit);
	return *cit->second;
}

//////////////////////////////////////////////////////////////////////////

LPCSTR InventoryUtilities::GetGoodwillAsText(CHARACTER_GOODWILL goodwill)
{
	InitCharacterInfoStrings();
	R_ASSERT(charInfoGoodwillStrings);

	CharInfoStrings::const_iterator cit = charInfoGoodwillStrings->upper_bound(goodwill);
	R_ASSERT(charInfoGoodwillStrings->end() != cit);
	return *cit->second;
}


//////////////////////////////////////////////////////////////////////////
// специальная функция для передачи info_portions при нажатии кнопок UI 
// (для tutorial)
void InventoryUtilities::SendInfoToActor(LPCSTR info_id)
{
	if (GameID() != GAME_SINGLE) return;
	
	CActor* actor = smart_cast<CActor*>(Level().CurrentEntity());
	if(actor)
	{
		actor->TransferInfo(info_id, true);
	}
}

u32 InventoryUtilities::GetGoodwillColor(CHARACTER_GOODWILL gw)
{
	u32 res = 0xffc0c0c0;
	if(gw==NEUTRAL_GOODWILL){
		res = 0xffc0c0c0;
	}else
	if(gw>1000){
		res = 0xff00ff00;
	}else
	if(gw<-1000){
		res = 0xffff0000;
	}
	return res;
}

u32 InventoryUtilities::GetReputationColor(CHARACTER_REPUTATION_VALUE rv)
{
	u32 res = 0xffc0c0c0;
	if(rv==NEUTAL_REPUTATION){
		res = 0xffc0c0c0;
	}else
	if(rv>50){
		res = 0xff00ff00;
	}else
	if(rv<-50){
		res = 0xffff0000;
	}
	return res;
}

u32	InventoryUtilities::GetRelationColor(ALife::ERelationType relation)
{
	switch(relation) {
	case ALife::eRelationTypeFriend:
		return 0xff00ff00;
		break;
	case ALife::eRelationTypeNeutral:
		return 0xffc0c0c0;
		break;
	case ALife::eRelationTypeEnemy:
		return  0xffff0000;
		break;
	default:
		NODEFAULT;
	}
#ifdef DEBUG
	return 0xffffffff;
#endif
}
