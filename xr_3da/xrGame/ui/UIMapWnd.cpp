// CUIMapWnd.h:  диалог итерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapWnd.h"

#include "../HUDManager.h"

#include "../actor.h"
#include "../level.h"

#include "../pda.h"
#include "../infoportion.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;


#define MAP_LEFT		89
#define MAP_TOP			93
#define MAP_RADIUS		80


//задаются координаты карты из LevelEditor
//x1 - лево, x2 - право, 
//z1 - низ , z2 - верх


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapWnd::CUIMapWnd()
{
	Hide();
	
	SetFont(HUD().pFontSmall);

//	Init();
}

CUIMapWnd::~CUIMapWnd()
{
	RemoveAllSpots();
}

void CUIMapWnd::Show()
{
	InitMap();
	inherited::Show();
}
void CUIMapWnd::RemoveAllSpots()
{
	for(u32 i=0; i<UIMapBackground.m_vMapSpots.size(); ++i)
	{	
		UIMapBackground.m_vMapSpots[i]->GetParent()->DetachChild(UIMapBackground.m_vMapSpots[i]);
		xr_delete(UIMapBackground.m_vMapSpots[i]);
	}

	UIMapBackground.m_vMapSpots.clear();
}

void CUIMapWnd::Init()
{
	CUIXmlInit xml_init;
	CUIXml uiXml;
	uiXml.Init("$game_data$","map.xml");

	inherited::Init(0, 0, Device.dwWidth, Device.dwHeight);

//	AttachChild(&UIStaticTop);
//	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
//	AttachChild(&UIStaticBottom);
//	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

//	AttachChild(&UIMainMapFrame);
//	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainMapFrame);
//	UIMainMapFrame.AttachChild(&UIPDAHeader);
//	xml_init.InitStatic(uiXml, "static", 0, &UIPDAHeader);


	AttachChild(&UICheckButton1);
	xml_init.InitButton(uiXml, "button", 0, &UICheckButton1);
	AttachChild(&UICheckButton2);
	xml_init.InitButton(uiXml, "button", 1, &UICheckButton2);
	AttachChild(&UICheckButton3);
	xml_init.InitButton(uiXml, "button", 2, &UICheckButton3);
	AttachChild(&UICheckButton4);
	xml_init.InitButton(uiXml, "button", 3, &UICheckButton4);

	UICheckButton1.Show(false);UICheckButton1.Enable(false);
	UICheckButton2.Show(false);UICheckButton2.Enable(false);
	UICheckButton3.Show(false);UICheckButton3.Enable(false);
	UICheckButton4.Show(false);UICheckButton4.Enable(false);

	//информация о точке на карте на которую наводит игрок
	AttachChild(&UIStaticInfo);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticInfo);

	UIStaticInfo.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0, UIStaticInfo.GetWidth(), UIStaticInfo.GetHeight(), "map_character.xml");

	
	AttachChild(&UIMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UIMapBackground);
	UIMapBackground.Show(true);
	UIMapBackground.Enable(true);
	
	m_iMapWidth = UIMapBackground.GetWidth();
	m_iMapHeight = UIMapBackground.GetHeight();

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);
}

void CUIMapWnd::InitMap()
{
	UIMapBackground.m_pActiveMapSpot = NULL;

	if (Level().pLevel->section_exist("level_map"))	
	{
		UIMapBackground.m_LevelBox.x1 = Level().pLevel->r_float("level_map","x1");
		UIMapBackground.m_LevelBox.z1 = Level().pLevel->r_float("level_map","z1");
		UIMapBackground.m_LevelBox.x2 = Level().pLevel->r_float("level_map","x2");
		UIMapBackground.m_LevelBox.z2 = Level().pLevel->r_float("level_map","z2");
		UIMapBackground.m_MapTextureName = Level().pLevel->r_string("level_map","texture");
	}
	else
	{
		//в отладочных целях задаем хоть какой-то размер уровня
		UIMapBackground.m_LevelBox.x1 = -443.66000f;
		UIMapBackground.m_LevelBox.z1 = -131.66000f;
		UIMapBackground.m_LevelBox.x2 = 196.33000f;
		UIMapBackground.m_LevelBox.z2 = 508.32999f;
	}

	m_fWorldMapWidth = UIMapBackground.m_LevelBox.x2 - UIMapBackground.m_LevelBox.x1;
	m_fWorldMapHeight = UIMapBackground.m_LevelBox.z2 - UIMapBackground.m_LevelBox.z1;

	m_fWorldMapLeft = UIMapBackground.m_LevelBox.x1;
	m_fWorldMapTop = UIMapBackground.m_LevelBox.z2;


	UIMapBackground.InitMapBackground();

	///убрать все интерактивные отметки на карте 
	RemoveAllSpots();

	Ivector2 P;
	Fvector src;
	
	CActor* pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
	if (!pActor) return;

//	CTeam& Team = Level().Teams[pActor->id_Team];

	int left = UIMapBackground.GetWndRect().left;
	int top = UIMapBackground.GetWndRect().top;



	////////////////////////////////////////////
	//добавить локации, имеющиеся в памяти PDA
	////////////////////////////////////////////
	
	for(KNOWN_INFO_PAIR_IT it = pActor->GetPDA()->m_mapKnownInfo.begin();
		pActor->GetPDA()->m_mapKnownInfo.end() != it;
		++it)
	{
		//подгрузить кусочек информации с которым мы работаем
		CInfoPortion info_portion;
		info_portion.Load((*it).first);

		//локации заданные в порцие информации	
		for(int i=0; i<info_portion.GetLocationsNum(); i++)
		{
			CUIMapSpot* map_spot;
			map_spot = xr_new<CUIMapSpot>();

			SMapLocation& map_location = info_portion.GetLocation(i);

			if(map_location.attached_to_object)
			{
				map_spot->m_pObject = Level().Objects.net_Find	(map_location.object_id);

				src.x = map_spot->m_pObject->Position().x;
				src.y = 0;
				src.z = map_spot->m_pObject->Position().z;
			}
			else
			{
				src.x = map_location.x;
				src.y = 0;
				src.z = map_location.y;

				map_spot->m_pObject = NULL;
				map_spot->m_vWorldPos.set(map_location.x,0.f,map_location.y);
			}

			UIMapBackground.ConvertToLocal(src,P);

			map_spot->SetShader(GetCharIconsShader());
			map_spot->MoveWindow(P.x + left, P.y + top);
			map_spot->SetWidth(MAP_ICON_WIDTH);
			map_spot->SetHeight(MAP_ICON_HEIGHT);
			map_spot->GetUIStaticItem().SetOriginalRect(
								map_location.icon_x*ICON_GRID_WIDTH,
								map_location.icon_y*ICON_GRID_HEIGHT,
								map_location.icon_width*ICON_GRID_WIDTH,
								map_location.icon_height*ICON_GRID_HEIGHT);
			
			map_spot->m_bCenter = true;
			map_spot->m_sDescText.SetText("");
			map_spot->m_sNameText.SetText("");
			if(xr_strlen(map_location.text)>1)
			{
				map_spot->m_sDescText = map_location.text;
				map_spot->m_sNameText = map_location.name;
			}


			UIMapBackground.m_vMapSpots.push_back(map_spot);
			UIMapBackground.AttachChild(map_spot);
		}
	}


	////////////////////////////////
	//добавить на карту актера
	////////////////////////////////

    UIMapBackground.ConvertToLocal(pActor->Position(),P);
	CUIMapSpot* map_spot;
	map_spot = xr_new<CUIMapSpot>();
	map_spot->m_pObject = pActor;
	
	map_spot->SetShader(GetCharIconsShader());
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);
	map_spot->MoveWindow(P.x + left, P.y + top);
	map_spot->m_bCenter = true;
	map_spot->m_sDescText.SetText("It's you.");
	map_spot->m_sNameText.SetText("You");

	map_spot->GetUIStaticItem().SetOriginalRect(
						pActor->GetMapIconX()*ICON_GRID_WIDTH,
						pActor->GetMapIconY()*ICON_GRID_HEIGHT,
						(pActor->GetMapIconX()+1)*ICON_GRID_WIDTH,
						(pActor->GetMapIconY()+1)*ICON_GRID_HEIGHT);

	UIMapBackground.m_vMapSpots.push_back(map_spot);
	UIMapBackground.AttachChild(map_spot);

	//информация о выбранном объекте
	UIStaticInfo.Show(false);
	UICharacterInfo.Show(false);
}

void CUIMapWnd::AddObjectSpot(CGameObject* pGameObject)
{	
	//!!! пока только для CEntity
	CEntity* pEntity = dynamic_cast<CEntity*>(pGameObject);
	if(!pEntity) return;


	int left = UIMapBackground.GetWndRect().left;
	int top = UIMapBackground.GetWndRect().top;

	Ivector2 P;
    UIMapBackground.ConvertToLocal(pGameObject->Position(),P);
	
	CUIMapSpot* map_spot;
	map_spot = xr_new<CUIMapSpot>();
	map_spot->m_pObject = pEntity;
	
	map_spot->SetShader(GetCharIconsShader());
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);
	map_spot->MoveWindow(P.x + left, P.y + top);
	map_spot->m_bCenter = true;
	map_spot->m_sDescText.SetText("");
	map_spot->m_sNameText.SetText("");

	map_spot->GetUIStaticItem().SetOriginalRect(
						pEntity->GetMapIconX()*ICON_GRID_WIDTH,
						pEntity->GetMapIconY()*ICON_GRID_HEIGHT,
						(pEntity->GetMapIconX()+1)*ICON_GRID_WIDTH,
						(pEntity->GetMapIconY()+1)*ICON_GRID_HEIGHT);

	UIMapBackground.m_vMapSpots.push_back(map_spot);
	UIMapBackground.AttachChild(map_spot);
}

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UIMapBackground)
	{
		if(CUIMapBackground::MAPSPOT_FOCUS_RECEIVED == msg)
		{
			if(UIMapBackground.m_pActiveMapSpot)
			{
				UIStaticInfo.Show(true);
				
				CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(UIMapBackground.m_pActiveMapSpot->m_pObject);

				if(xr_strlen(UIMapBackground.m_pActiveMapSpot->m_sDescText.GetBuf())>1)
				{
					UICharacterInfo.Show(true);
					UICharacterInfo.ResetAllStrings();

					UICharacterInfo.UIName.SetText(UIMapBackground.m_pActiveMapSpot->m_sNameText);
					UICharacterInfo.UIText.SetText(UIMapBackground.m_pActiveMapSpot->m_sDescText);
					
				}
				else
				{
					UICharacterInfo.ResetAllStrings();
					if(pInvOwner)
					{
						UICharacterInfo.InitCharacter(pInvOwner);
						UICharacterInfo.Show(true);
					}
					else
						UICharacterInfo.Show(false);
				}
            }
		}
		else if(CUIMapBackground::MAPSPOT_FOCUS_LOST == msg)
		{
			UIStaticInfo.Show(false);
		}
	}
	inherited::SendMessage(pWnd, msg, pData);
}


void CUIMapWnd::Update()
{
	inherited::Update();
}

void CUIMapWnd::Draw()
{
	inherited::Draw();
}

//перевод из мировой системы координат в координаты карты
//с началом координат в левом верхнем углу
void CUIMapWnd::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = iFloor(0.5f + m_iMapWidth*(src.x - m_fWorldMapLeft)/m_fWorldMapWidth);
	dest.y = m_iMapHeight - iFloor(0.5f + m_iMapHeight*(src.z - m_fWorldMapTop)/m_fWorldMapHeight);
}

//прорисовка карты и объектов на ней
void CUIMapWnd::DrawMap()
{
}

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	UIMapBackground.SetActivePos(vNewPoint);
	UIMapBackground.m_bNoActorFocus = true;
}
