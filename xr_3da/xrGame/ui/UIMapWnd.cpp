// CUIMapWnd.cpp:  диалог интерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../HUDManager.h"
#include "../actor.h"
#include "../pda.h"
#include "../infoportion.h"
#include "../level.h"
#include "../string_table.h"
#include "../UIZoneMap.h"
#include "UIInventoryUtilities.h"
#include "UIPdaWnd.h"
#include "UIXmlInit.h"
#include "UIMapWnd.h"

//////////////////////////////////////////////////////////////////////////

using namespace InventoryUtilities;

#define				MAP_LEFT					89
#define				MAP_TOP						93
#define				MAP_RADIUS					80


//-----------------------------------------------------------------------------/
//	Задаются координаты карты в game.ltx запись ui_map_dims
//	лево, право, низ, верх
//-----------------------------------------------------------------------------/

const char * const	PDA_MAP_XML					= "map.xml";
const				SCROLLBARS_SHIFT			= 5;
const				VSCROLLBAR_STEP				= 20; // В пикселях
const				HSCROLLBAR_STEP				= 20; // В пикселях

//-----------------------------------------------------------------------------/
//	Global map stuff
//-----------------------------------------------------------------------------/

const char * const	GLOBAL_MAP_LTX				= "global_map";
const char * const	GLOBAL_MAP_TEX_LTX			= "texture";
const char * const	GLOBAL_MAP_W_LTX			= "width";
const char * const	GLOBAL_MAP_H_LTX			= "height";
const char * const	GLOBAL_MAP_LOCATIONS_LTX	= "global_map_locations";

const u32			activeLocalMapColor			= 0xff00ff00;
const u32			inactiveLocalMapColor		= 0xff0000ff;
const u32			ourLevelMapColor			= 0xffffffff;
const int			globalMapButtonWidth		= 128;
const int			globalMapButtonHeight		= 256;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapWnd::CUIMapWnd()
	:	m_eCurrentMode		(emmLocal),
		m_pCurrentMap		(&UILocalMapBackground),
		m_GlobalMapHeight	(0.0f),
		m_GlobalMapWidth	(0.0f),
		m_GlobalMapShader	(NULL)
{
	Show					(false);
	SetFont					(HUD().pFontSmall);
	m_rDefault.left			= 0;
	m_rDefault.top			= 0;
	m_rDefault.bottom		= 100;
	m_rDefault.right		= 100;
}

//////////////////////////////////////////////////////////////////////////

CUIMapWnd::~CUIMapWnd()
{
	UILocalMapBackground.RemoveAllSpots	();
	UIGlobalMapBackground.RemoveAllSpots();
	DeleteLocalMapsData					();
	m_GlobalMapShader.destroy			();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Show(bool status)
{
	if (status)
	{
		InitGlobalMapObjectives			();
		InitLocalMapObjectives			();
		m_eCurrentMode = emmGlobal;
		SwitchMapMode					(emmLocal);
		SetLocalMap						(Level().name());
	}
	inherited::Show(status);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Init()
{
	CUIXmlInit xml_init;
	CUIXml uiXml;
	uiXml.Init("$game_data$", PDA_MAP_XML);

	inherited::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIMainMapFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainMapFrame);
	UIMainMapFrame.AttachChild(&UIPDAHeader);
	xml_init.InitFrameLine(uiXml, "map_header_frame_line", 0, &UIPDAHeader);

	string128 buf;
	strconcat(buf, ALL_PDA_HEADER_PREFIX, "/Map");

	UIPDAHeader.UITitleText.SetText(buf);

	//информация о точке на карте на которую наводит игрок
	AttachChild(&UIStaticInfo);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticInfo);

	UIStaticInfo.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0, UIStaticInfo.GetWidth(), UIStaticInfo.GetHeight(), "map_character.xml");
	UIStaticInfo.Enable(false);
	
	UIMainMapFrame.AttachChild(&UILocalMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UILocalMapBackground);
	UILocalMapBackground.SetMessageTarget(this);
	UILocalMapBackground.Enable(true);

	UIMainMapFrame.AttachChild(&UIGlobalMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UIGlobalMapBackground);
	UIGlobalMapBackground.SetMessageTarget(this);
	UIGlobalMapBackground.Enable(true);
	UIMainMapFrame.DetachChild(&UIGlobalMapBackground);

	// Descriptions
	UIGlobalMapBackground.AttachChild(&UIMapName);
	xml_init.InitStatic(uiXml, "map_name_static", 0, &UIMapName);
	UIMapName.Enable(false);
	UIMapName.Show(false);

	UIGlobalMapBackground.AttachChild(&UIMapGoals);
	xml_init.InitListWnd(uiXml, "map_goals_list", 0, &UIMapGoals);
	UIMapGoals.Enable(false);
	UIMapGoals.Show(false);

	RECT r = UILocalMapBackground.GetWndRect();

	UIMainMapFrame.AttachChild(&UIMapBgndV);
	UIMapBgndV.Init(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	UIMapBgndV.SetMessageTarget(this);

	UIMainMapFrame.AttachChild(&UIMapBgndH);
	UIMapBgndH.Init(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	UIMapBgndH.SetMessageTarget(this);

	// Кнопка переключения глобальная/локальная карта
	UILocalMapBackground.AttachChild(&UIMapTypeSwitch);
	xml_init.InitButton(uiXml, "gl_button", 0, &UIMapTypeSwitch);
	UIMapTypeSwitch.SetMessageTarget(this);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	m_GlobalMapWidth			= pSettings->r_float(GLOBAL_MAP_LTX, GLOBAL_MAP_W_LTX);
	m_GlobalMapHeight			= pSettings->r_float(GLOBAL_MAP_LTX, GLOBAL_MAP_H_LTX);

	// Инициализируем карты
	InitGlobalMap();
	InitLocalMaps();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SetLocalMap(const shared_str &levelName)
{
	UILocalMapBackground.m_pActiveMapSpot = NULL;

	LocalMaps_it currentMapIt = m_LocalMaps.find(levelName);
	R_ASSERT2(m_LocalMaps.end() != currentMapIt, "Map data for current level is absent.\nMost possible reason - this level not included in game graph");

	UILocalMapBackground.m_LevelBox.x1 = currentMapIt->second.mapDimentions.x;
	UILocalMapBackground.m_LevelBox.z1 = currentMapIt->second.mapDimentions.z;
	UILocalMapBackground.m_LevelBox.x2 = currentMapIt->second.mapDimentions.y;
	UILocalMapBackground.m_LevelBox.z2 = currentMapIt->second.mapDimentions.w;

	R_ASSERT(currentMapIt->second.mapTexture);
	UILocalMapBackground.InitMapBackground(currentMapIt->second.mapTexture);

	// Ставим фильтр на мапспоты
	ApplyFilterToObjectives(levelName);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitLocalMapObjectives()
{
	///убрать все интерактивные отметки на карте 
	UILocalMapBackground.RemoveAllSpots();

	Ivector2 P;
	Fvector src;

	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

	if (!pActor) return;

	int left	= UILocalMapBackground.GetWndRect().left;
	int top		= UILocalMapBackground.GetWndRect().top;

	////////////////////////////////////////////
	//добавить локации, имеющиеся в памяти PDA
	////////////////////////////////////////////

	for(LOCATIONS_PTR_VECTOR_IT it = Level().MapLocations().begin();
		Level().MapLocations().end() != it; 
		it++)
	{
		CUIMapSpot * map_spot;
		map_spot = xr_new<CUIMapSpot>();
		SMapLocation& map_location = *(*it);

		if(map_location.attached_to_object)
		{
			map_spot->SetObjectID(map_location.object_id);
			src = map_spot->MapPos();
		}
		else
		{
			src.x = map_location.x;
			src.y = 0;
			src.z = map_location.y;

			map_spot->SetObjectID(0xffff);
			map_spot->m_vWorldPos.set(map_location.x,0.f,map_location.y);
		}

		map_spot->arrow_color		= map_location.icon_color;
		map_spot->m_bArrowEnabled	= map_location.marker_show;
		map_spot->m_LevelName		= map_location.level_name;

		UILocalMapBackground.ConvertToLocal(src,P);

		map_spot->SetShader(GetMapIconsShader());
		map_spot->MoveWindow(P.x + left, P.y + top);
		map_spot->SetWidth(MAP_ICON_WIDTH*map_location.icon_width);
		map_spot->SetHeight(MAP_ICON_HEIGHT*map_location.icon_height);
		map_spot->GetUIStaticItem().SetOriginalRect(
			map_location.icon_x*MAP_ICON_GRID_WIDTH,
			map_location.icon_y*MAP_ICON_GRID_HEIGHT,
			map_location.icon_width*MAP_ICON_GRID_WIDTH + MAP_ICON_WIDTH,
			map_location.icon_height*MAP_ICON_GRID_HEIGHT + MAP_ICON_HEIGHT);

		map_spot->m_eAlign = CUIMapSpot::eCenter;
		map_spot->m_sDescText.SetText("");
		map_spot->m_sNameText.SetText("");
		map_spot->m_bArrowEnabled = true;

		if(xr_strlen(map_location.text)>1)
		{
			map_spot->m_sDescText.SetText(*map_location.text);
			map_spot->m_sNameText.SetText(*map_location.name);
		}

		UILocalMapBackground.m_vMapSpots.push_back(CUIMapBackground::MapSpotPtr(map_spot));
		UILocalMapBackground.AttachChild(map_spot);
	}

	//	Добавить на карту актера только если мы отображаем текущую карту

	UILocalMapBackground.ConvertToLocal(pActor->Position(),P);
	CUIMapSpot* map_spot;

	map_spot = xr_new<CUIMapSpot>();
	map_spot->SetObjectID(pActor->ID());

	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	map_spot->m_fHeading = h;
	map_spot->m_bHeading = true;

	map_spot->InitTexture(pSettings->r_string("game_map", "actor_arrow"));
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);
	map_spot->SetObjectID(0xffff);

	map_spot->MoveWindow(P.x + left, P.y + top);
	map_spot->m_eAlign = CUIMapSpot::eCenter;
	map_spot->m_sDescText.SetText("It's you.");
	map_spot->m_sNameText.SetText("You");
	map_spot->m_vWorldPos.set(pActor->Position());
	map_spot->m_LevelName = Level().name();

	UILocalMapBackground.m_vMapSpots.push_back(CUIMapBackground::MapSpotPtr(map_spot));
	UILocalMapBackground.AttachChild(map_spot);
	m_pActorSpot = map_spot;

	//!!! предварительный рендер неизвестно зачем
	m_pActorSpot->GetStaticItem()->Render();

	//информация о выбранном объекте
	UIStaticInfo.Show(false);
	UICharacterInfo.Show(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitGlobalMap()
{
	// Инициализируем параметры глобальной карты
	UIGlobalMapBackground.m_LevelBox.x1		= 0.0f;
	UIGlobalMapBackground.m_LevelBox.z1		= 0.0f;
	UIGlobalMapBackground.m_LevelBox.x2		= m_GlobalMapWidth;
	UIGlobalMapBackground.m_LevelBox.z2		= m_GlobalMapHeight;

	m_GlobalMapShader.create("hud\\default", pSettings->r_string(GLOBAL_MAP_LTX, GLOBAL_MAP_TEX_LTX));

	UIGlobalMapBackground.InitMapBackground(m_GlobalMapShader);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitGlobalMapObjectives()
{
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

	if (!pActor) return;
	LocalMaps_it mapIt;

	for (mapIt = m_LocalMaps.begin(); mapIt != m_LocalMaps.end(); ++mapIt)
	{
		LocalMapDef &l = mapIt->second;
		R_ASSERT				(l.mapSpot);
		l.obj.clear				();
		l.mapSpot->SetColor		(inactiveLocalMapColor);
		l.smallFrame->SetColor	(inactiveLocalMapColor);
	}

	// Добавить отметки на карте, которые актер помнит в info_portions
	if(pActor->known_info_registry.objects_ptr())
	{
		const KNOWN_INFO_VECTOR& know_info = *pActor->known_info_registry.objects_ptr();
		for(KNOWN_INFO_VECTOR::const_iterator it = know_info.begin();
			know_info.end() != it; it++)
		{
			CInfoPortion info_portion;
			info_portion.Load((*it).id);

			// Добавить отметки на карте
			for(u32 i=0; i<info_portion.MapLocations().size(); i++)
			{
				const SMapLocation& map_location = info_portion.MapLocations()[i];
				if(xr_strlen(*map_location.level_name) > 0)
				{
					mapIt = m_LocalMaps.find(map_location.level_name);
					if (m_LocalMaps.end() != mapIt)
					{
						mapIt->second.obj.push_back(map_location.text);
						RECT r	= mapIt->second.smallFrame->GetWndRect();
						r.top	= globalMapButtonHeight - r.top;
						mapIt->second.visible = !(r.left == m_rDefault.left && r.top == m_rDefault.top);
					}
				}
			}
		}
	}

	// Даже если никаких локаций/квестов на карте на которой мы находимся нет, то все равно показать
	// карту
	mapIt = m_LocalMaps.find(Level().name());
	if (m_LocalMaps.end() != mapIt)
	{
		RECT r	= mapIt->second.smallFrame->GetWndRect();
		r.top	= globalMapButtonHeight - r.top;
		mapIt->second.visible = !(r.left == m_rDefault.left && r.top == m_rDefault.top);
		mapIt->second.smallFrame->SetColor(ourLevelMapColor);
		mapIt->second.mapSpot->SetColor(ourLevelMapColor);
	}

	// Показать ищвестные нам локации
	for (mapIt = m_LocalMaps.begin(); mapIt != m_LocalMaps.end(); ++mapIt)
	{
		LocalMapDef &l = mapIt->second;
		if (l.visible)
		{
			if (!UIGlobalMapBackground.IsChild(l.mapSpot.get()))
				UIGlobalMapBackground.AttachChild(l.mapSpot.get());
			if (!UIMapTypeSwitch.IsChild(l.smallFrame))
				UIMapTypeSwitch.AttachChild(l.smallFrame);
		}
		else
		{
			if (UIGlobalMapBackground.IsChild(l.mapSpot.get()))
				UIGlobalMapBackground.DetachChild(l.mapSpot.get());
			if (UIMapTypeSwitch.IsChild(l.smallFrame))
				UIMapTypeSwitch.DetachChild(l.smallFrame);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::AddObjectSpot(CGameObject* pGameObject)
{	
	//!!! пока только для CEntity (чтоб было откуда брать иконку)
	CEntity* pEntity = smart_cast<CEntity*>(pGameObject);
	if(!pEntity) return;


	int left	= UILocalMapBackground.GetWndRect().left;
	int top		= UILocalMapBackground.GetWndRect().top;

	Ivector2 P;
    UILocalMapBackground.ConvertToLocal(pGameObject->Position(),P);
	
	CUIMapSpot* map_spot;
	map_spot = xr_new<CUIMapSpot>();
	map_spot->SetObjectID(pEntity->ID());
	
	map_spot->SetShader(GetCharIconsShader());
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);
	map_spot->MoveWindow(P.x + left, P.y + top);
	map_spot->m_eAlign = CUIMapSpot::eCenter;
	map_spot->m_sDescText.SetText("");
	map_spot->m_sNameText.SetText("");
	map_spot->m_LevelName = Level().name();

	map_spot->GetUIStaticItem().SetOriginalRect(
						pEntity->GetMapIconX()*ICON_GRID_WIDTH + MAP_ICON_WIDTH / 2,
						pEntity->GetMapIconY()*ICON_GRID_HEIGHT + MAP_ICON_HEIGHT,
						(pEntity->GetMapIconX()+1)*ICON_GRID_WIDTH,
						(pEntity->GetMapIconY()+1)*ICON_GRID_HEIGHT);

	UILocalMapBackground.m_vMapSpots.push_back(CUIMapBackground::MapSpotPtr(map_spot));
	UILocalMapBackground.AttachChild(map_spot);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	static u16 prevScrollPosV = 0;
	static u16 prevScrollPosH = 0;

	if(pWnd == &UILocalMapBackground)
	{
		if(MAPSPOT_FOCUS_RECEIVED == msg)
		{
			if(m_pCurrentMap->m_pActiveMapSpot)
			{
				UIStaticInfo.Show(true);
				
				//CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_pCurrentMap->m_pActiveMapSpot->m_pObject);

				if(xr_strlen(m_pCurrentMap->m_pActiveMapSpot->m_sDescText.GetBuf())>1)
				{
					UICharacterInfo.Show(true);
					UICharacterInfo.ResetAllStrings();

					UICharacterInfo.UIName.SetText(m_pCurrentMap->m_pActiveMapSpot->m_sNameText);
					UICharacterInfo.UIRank.SetText(m_pCurrentMap->m_pActiveMapSpot->m_sDescText);
					
				}
				else
				{
					UICharacterInfo.ResetAllStrings();
/*					if(pInvOwner)
					{
						UICharacterInfo.InitCharacter(pInvOwner);
						UICharacterInfo.Show(true);
					}
					else*/
					UICharacterInfo.Show(false);
				}
            }
		}
		else if(MAPSPOT_FOCUS_LOST == msg)
		{
			UIStaticInfo.Show(false);
		}
	}

	if (&UIGlobalMapBackground == pWnd)
	{
		static CUIMapSpot *prevSpot = NULL;
		if (MAPSPOT_FOCUS_RECEIVED == msg)
		{
			if (UIGlobalMapBackground.m_pActiveMapSpot && prevSpot != UIGlobalMapBackground.m_pActiveMapSpot)
			{
				CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
				R_ASSERT(pGML);
				pGML->SetColor(activeLocalMapColor);
				UIMapName.SetText(*pGML->m_strMapName);
				// Show map goals
				for (Objectives_it it = m_LocalMaps[pGML->m_strMapName].obj.begin(); it != m_LocalMaps[pGML->m_strMapName].obj.end(); ++it)
				{
					UIMapGoals.AddItem<CUIListItem>(static_cast<const char *>(*(*it)));
				}
				prevSpot = UIGlobalMapBackground.m_pActiveMapSpot;
			}
			UIMapGoals.Show(true);
			UIMapName.Show(true);
		}

		if (MAPSPOT_FOCUS_LOST == msg)
		{
			UIMapGoals.Show(false);
			UIMapGoals.RemoveAll();
			prevSpot = NULL;
			if (UIGlobalMapBackground.m_pActiveMapSpot)
			{
				CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
				R_ASSERT(pGML);
				if (Level().name() == pGML->m_strMapName)
					pGML->SetColor(ourLevelMapColor);
				else
					pGML->SetColor(inactiveLocalMapColor);
			}
			UIMapName.Show(false);
		}

		if (MAPSPOT_CLICKED == msg)
		{
			CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
			if (pGML)
			{
				SetLocalMap(pGML->m_strMapName);
				SwitchMapMode(emmLocal);
			}
		}
	}

	if (&UIMapBgndV == pWnd)
	{
		if (SCROLLBAR_VSCROLL == msg)
		{
			m_pCurrentMap->MoveMap(0, -VSCROLLBAR_STEP * (UIMapBgndV.GetScrollPos() - prevScrollPosV));
			prevScrollPosV = UIMapBgndV.GetScrollPos();
			m_pCurrentMap->Update();
		}
	}

	if (&UIMapBgndH == pWnd)
	{
		if (SCROLLBAR_HSCROLL == msg)
		{
			m_pCurrentMap->MoveMap(-HSCROLLBAR_STEP * (UIMapBgndH.GetScrollPos() - prevScrollPosH), 0);
			prevScrollPosH = UIMapBgndH.GetScrollPos();
			m_pCurrentMap->Update();
		}
	}

	if (m_pCurrentMap == pWnd && MAP_MOVED == msg)
	{
		Fvector2	dest;
		m_pCurrentMap->ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);
		if (UIMapBgndV.IsShown())
			UIMapBgndV.SetScrollPos(static_cast<s16>(UIMapBgndV.GetMaxRange() - iCeil((m_pCurrentMap->m_fMapHeightMeters - m_pCurrentMap->m_fMapViewHeightMeters - m_pCurrentMap->m_fMapY) / dest.y)));
		if (UIMapBgndH.IsShown())
			UIMapBgndH.SetScrollPos(static_cast<s16>(UIMapBgndH.GetMaxRange() - iCeil((m_pCurrentMap->m_fMapWidthMeters - m_pCurrentMap->m_fMapViewWidthMeters - m_pCurrentMap->m_fMapX) / dest.x)));

		prevScrollPosH = UIMapBgndH.GetScrollPos();
		prevScrollPosV = UIMapBgndV.GetScrollPos();
		m_pCurrentMap->Update();
	}
	
	if (&UIMapTypeSwitch == pWnd && BUTTON_CLICKED == msg)
	{
		SwitchMapMode(emmGlobal);
		for (WINDOW_LIST_it it = UIMapTypeSwitch.GetChildWndList().begin();
							it != UIMapTypeSwitch.GetChildWndList().end();
							++it)
		{
			RECT r = (*it)->GetWndRect();
			(*it)->MoveWindow(r.left - UIMapTypeSwitch.GetPushOffsetX(), r.top - UIMapTypeSwitch.GetPushOffsetY());
		}
	}
	else if (&UIMapTypeSwitch == pWnd && BUTTON_DOWN == msg)
	{
		for (WINDOW_LIST_it it = UIMapTypeSwitch.GetChildWndList().begin();
							it != UIMapTypeSwitch.GetChildWndList().end();
							++it)
		{
			RECT r = (*it)->GetWndRect();
			(*it)->MoveWindow(r.left + UIMapTypeSwitch.GetPushOffsetX(), r.top + UIMapTypeSwitch.GetPushOffsetY());
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Update()
{
	float h,p;

	Device.vCameraDirection.getHP	(h,p);
	m_pActorSpot->m_fHeading = h;
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
	m_pActorSpot->m_vWorldPos.set(pActor->Position());

	inherited::Update();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Draw()
{
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////
//	перевод из мировой системы координат в координаты карты
//	с началом координат в левом верхнем углу
//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = iFloor(0.5f + m_iMapWidth*(src.x - m_fWorldMapLeft)/m_fWorldMapWidth);
	dest.y = m_iMapHeight - iFloor(0.5f + m_iMapHeight*(src.z - m_fWorldMapTop)/m_fWorldMapHeight);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	UILocalMapBackground.SetActivePos(vNewPoint);
	UILocalMapBackground.m_bNoActorFocus = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SwitchMapMode(const EMapModes mode)
{
	// Проверяем на переключение в текущий режим
	if (mode == m_eCurrentMode) return;

	m_eCurrentMode = mode;

	CStringTable st;

	UIMainMapFrame.DetachChild(m_pCurrentMap);

	switch (m_eCurrentMode)
	{
	case emmLocal:
//		UIMapTypeSwitch.SetText(*st("Global Map"));
		m_pCurrentMap = &UILocalMapBackground;
		UILocalMapBackground.Show(true);
		break;
	case emmGlobal:
//		UIMapTypeSwitch.SetText(*st("Local Map"));
		m_pCurrentMap = &UIGlobalMapBackground;
		UIGlobalMapBackground.Show(true);
		for (LocalMaps_it it = m_LocalMaps.begin(); it != m_LocalMaps.end(); ++it)
		{
			if (Level().name() == it->first)
				it->second.mapSpot->SetColor(ourLevelMapColor);
			else
				it->second.mapSpot->SetColor(inactiveLocalMapColor);
		}
		break;
	default:
		NODEFAULT;
	}    

	UIMainMapFrame.AttachChild(m_pCurrentMap);

	m_fWorldMapWidth = m_pCurrentMap->m_LevelBox.x2 - m_pCurrentMap->m_LevelBox.x1;
	m_fWorldMapHeight = m_pCurrentMap->m_LevelBox.z2 - m_pCurrentMap->m_LevelBox.z1;

	m_fWorldMapLeft = m_pCurrentMap->m_LevelBox.x1;
	m_fWorldMapTop = m_pCurrentMap->m_LevelBox.z2;

	Fvector2	dest;
	m_pCurrentMap->ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);

	// Настраиваем скроллбоксы
	if (m_pCurrentMap->m_fMapHeightMeters > m_pCurrentMap->m_fMapViewHeightMeters)
	{
		UIMapBgndV.Show(true);
		UIMapBgndV.SetRange(0, static_cast<s16>(iCeil((m_pCurrentMap->m_fMapHeightMeters - m_pCurrentMap->m_fMapViewHeightMeters) / dest.y)));
	}
	else
		UIMapBgndV.Show(false);

	if (m_pCurrentMap->m_fMapWidthMeters > m_pCurrentMap->m_fMapViewWidthMeters)
	{
		UIMapBgndH.Show(true);
		UIMapBgndH.SetRange(0, static_cast<s16>(iCeil((m_pCurrentMap->m_fMapWidthMeters - m_pCurrentMap->m_fMapViewWidthMeters) / dest.x)));
	}
	else
		UIMapBgndH.Show(false);

	SendMessage(m_pCurrentMap, MAP_MOVED, NULL);

	m_iMapWidth = m_pCurrentMap->GetWidth();
	m_iMapHeight = m_pCurrentMap->GetHeight();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::AddLocalMap(const shared_str levelName, const Ivector4 &v, const Fvector4 &d, const shared_str &textureName)
{
	R_ASSERT	(v[2] > 0);
	R_ASSERT	(v[3] > 0);
	
	// If level with this name already exists
	R_ASSERT2	(m_LocalMaps.end() == m_LocalMaps.find(levelName), "Duplicate level name not allowed");
	LocalMapDef &currentLevelMap = m_LocalMaps[levelName];

	// Имя текстуры фрейма для локальных карт
	const char * const localMapFrameTexture = "ui\\ui_frame_very_small";

	Ivector4	iTmpV;
	iTmpV[2] = static_cast<int>(v[2] * UIGlobalMapBackground.m_iMapViewWidthPixels	/ UIGlobalMapBackground.m_fMapViewWidthMeters);
	iTmpV[3] = static_cast<int>(v[3] * UIGlobalMapBackground.m_iMapViewHeightPixels	/ UIGlobalMapBackground.m_fMapViewHeightMeters);

	float		xSmallMapFactor		= m_GlobalMapWidth	/ globalMapButtonWidth;
	float		ySmallMapFactor		= m_GlobalMapHeight / globalMapButtonHeight;

	// Create local map rect on global map
	LocalMapDef::GMLptr pGML		(xr_new<CUIGlobalMapLocation>());
	pGML->Init(iTmpV[2], iTmpV[3], UIGlobalMapBackground.GetAbsoluteRect(), localMapFrameTexture);
	pGML->m_vWorldPos.x				= static_cast<float>(v[0]);
	pGML->m_vWorldPos.z				= static_cast<float>(v[1]);
	pGML->SetColor(inactiveLocalMapColor);
	pGML->m_strMapName				= CStringTable()(levelName);
	currentLevelMap.mapSpot			= pGML;
	UIGlobalMapBackground.m_vMapSpots.push_back(pGML);
//	UIGlobalMapBackground.AttachChild(pGML.get());

	// Add map locations to global map button
	iTmpV[0] = static_cast<int>(v[0] / xSmallMapFactor);
	iTmpV[1] = static_cast<int>(v[1] / ySmallMapFactor);
	iTmpV[2] = static_cast<int>(v[2] / xSmallMapFactor);
	iTmpV[3] = static_cast<int>(v[3] / ySmallMapFactor);

	currentLevelMap.smallFrame = xr_new<CUIFrameWindow>();
	currentLevelMap.smallFrame->Init(localMapFrameTexture, iTmpV[0], globalMapButtonHeight - iTmpV[1], iTmpV[2], iTmpV[3]);
	currentLevelMap.smallFrame->SetColor(inactiveLocalMapColor);
//	UIMapTypeSwitch.AttachChild(currentLevelMap.smallFrame);

	// Initialize map itself
	currentLevelMap.mapTexture.create("hud\\default", *textureName);
	currentLevelMap.mapDimentions.set(d);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitLocalMaps()
{
	R_ASSERT(ai().get_game_graph());

	// Получаем список уровней игры
	const CGameGraph::LEVEL_MAP &levelMap		= ai().get_game_graph()->header().levels();
	
	string256			gameLtxPath;
	FS.update_path					(gameLtxPath, "$game_data$", "game.ltx");
	CInifile			gameLtx		(gameLtxPath);
	// Loop through all levels in graph and initialize its map data
	for (CGameGraph::LEVEL_MAP::const_iterator it = levelMap.begin(); it != levelMap.end(); ++it)
	{
		shared_str		levelLtxRecord	= it->second.section	();
 		string32    levelName;
		strcpy		(levelName, gameLtx.r_string(levelLtxRecord, "name"));
		_strlwr		(levelName);
		

		Ivector4	mapCoords;	
		shared_str		mapTextureName;
		Fvector4	mapDims;

		if (gameLtx.line_exist(levelLtxRecord, mapTextureKey))
		{
			mapTextureName	= gameLtx.r_string		(levelLtxRecord, mapTextureKey);

			R_ASSERT2(gameLtx.line_exist(levelLtxRecord, mapLocalCoordinatesKey), "Local map on global map position data is absent");
			mapCoords		= gameLtx.r_ivector4	(levelLtxRecord, mapLocalCoordinatesKey);

			R_ASSERT2(gameLtx.line_exist(levelLtxRecord, mapDimentions), "Local map dimentions data is absent");
			mapDims			= gameLtx.r_fvector4	(levelLtxRecord, mapDimentions);
		}
		else
		{
			// Подставляем фейк
			mapTextureName	= "ui\\ui_nomap";
			mapCoords.set	(m_rDefault.left, m_rDefault.top, m_rDefault.right, m_rDefault.bottom);
            mapDims.set		(-240.0f, 364.0f, -390.0f, 256.0f);
		}

		AddLocalMap(levelName, mapCoords, mapDims, mapTextureName);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::DeleteLocalMapsData()
{
	for (LocalMaps_it it = m_LocalMaps.begin(); it != m_LocalMaps.end(); ++it)
	{
		if (it->second.smallFrame->GetParent())
		{
			it->second.smallFrame->GetParent()->DetachChild(it->second.smallFrame);
		}
		xr_delete(it->second.smallFrame);
	}

	m_LocalMaps.clear();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::ApplyFilterToObjectives(const shared_str &levelName)
{
	for (CUIMapBackground::MAP_SPOT_VECTOR_IT mapSpotIt = UILocalMapBackground.m_vMapSpots.begin();
											  mapSpotIt != UILocalMapBackground.m_vMapSpots.end();
											  ++mapSpotIt)
	{
		(*mapSpotIt)->Show(levelName == (*mapSpotIt)->m_LevelName);
	}	
}