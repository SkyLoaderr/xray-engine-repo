// CUIMapWnd.h:  ������ ������������ �����
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapWnd.h"

#include "../HUDManager.h"

#include "../actor.h"

#include "../pda.h"
#include "../infoportion.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../level.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;


#define MAP_LEFT		89
#define MAP_TOP			93
#define MAP_RADIUS		80


//�������� ���������� ����� �� LevelEditor
//x1 - ����, x2 - �����, 
//z1 - ��� , z2 - ����

const char * const	PDA_MAP_XML			= "map.xml";
const				SCROLLBARS_SHIFT	= 5;
const				VSCROLLBAR_STEP		= 20; // � ��������
const				HSCROLLBAR_STEP		= 20; // � ��������

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
	uiXml.Init("$game_data$", PDA_MAP_XML);

	inherited::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

//	AttachChild(&UIStaticTop);
//	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
//	AttachChild(&UIStaticBottom);
//	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

	AttachChild(&UIMainMapFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainMapFrame);
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

	//���������� � ����� �� ����� �� ������� ������� �����
	AttachChild(&UIStaticInfo);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticInfo);

	UIStaticInfo.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0, UIStaticInfo.GetWidth(), UIStaticInfo.GetHeight(), "map_character.xml");
	
	UIMainMapFrame.AttachChild(&UIMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UIMapBackground);
	UIMapBackground.SetMessageTarget(this);
	UIMapBackground.Show(true);
	UIMapBackground.Enable(true);
	
	m_iMapWidth = UIMapBackground.GetWidth();
	m_iMapHeight = UIMapBackground.GetHeight();

	RECT r = UIMapBackground.GetWndRect();

	UIMainMapFrame.AttachChild(&UIMapBgndV);
	UIMapBgndV.Init(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	UIMapBgndV.SetMessageTarget(this);

	UIMainMapFrame.AttachChild(&UIMapBgndH);
	UIMapBgndH.Init(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	UIMapBgndH.SetMessageTarget(this);

	//�������� ��������������� ����������
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
		//� ���������� ����� ������ ���� �����-�� ������ ������
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
	Fvector2	dest;
	UIMapBackground.ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);

	// ����������� �����������
	if (UIMapBackground.m_fMapHeightMeters > UIMapBackground.m_fMapViewHeightMeters)
	{
		UIMapBgndV.Show(true);
		UIMapBgndV.SetRange(0, static_cast<s16>(iCeil((UIMapBackground.m_fMapHeightMeters - UIMapBackground.m_fMapViewHeightMeters) / dest.y)));
	}
	else
		UIMapBgndV.Show(false);

	if (UIMapBackground.m_fMapWidthMeters > UIMapBackground.m_fMapViewWidthMeters)
	{
		UIMapBgndH.Show(true);
		UIMapBgndH.SetRange(0, static_cast<s16>(iCeil((UIMapBackground.m_fMapWidthMeters - UIMapBackground.m_fMapViewWidthMeters) / dest.x)));
	}
	else
		UIMapBgndH.Show(false);

	SendMessage(&UIMapBackground, CUIMapBackground::MAP_MOVED, NULL);

	///������ ��� ������������� ������� �� ����� 
	RemoveAllSpots();

	Ivector2 P;
	Fvector src;
	
	CActor* pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
	if (!pActor) return;

//	CTeam& Team = Level().Teams[pActor->id_Team];

	int left = UIMapBackground.GetWndRect().left;
	int top = UIMapBackground.GetWndRect().top;



	////////////////////////////////////////////
	//�������� �������, ��������� � ������ PDA
	////////////////////////////////////////////
	for(LOCATIONS_PTR_VECTOR_IT it = Level().MapLocations().begin();
		Level().MapLocations().end() != it; 
		it++)
	{
		CUIMapSpot* map_spot;
		map_spot = xr_new<CUIMapSpot>();

		SMapLocation& map_location = *(*it);

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


	////////////////////////////////
	//�������� �� ����� ������
	////////////////////////////////

    UIMapBackground.ConvertToLocal(pActor->Position(),P);
	CUIMapSpot* map_spot;
	
	map_spot = xr_new<CUIMapSpot>();
	map_spot->m_pObject = pActor;

	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	map_spot->m_fHeading = h;
	
	//map_spot->SetShader(GetCharIconsShader());
	map_spot->InitTexture(pSettings->r_string("game_map", "actor_arrow"));
	//map_spot->SetWidth(MAP_ICON_WIDTH);
	//map_spot->SetHeight(MAP_ICON_HEIGHT);

	map_spot->SetWidth(32);
	map_spot->SetHeight(32);

	map_spot->MoveWindow(P.x + left, P.y + top);
	map_spot->m_bCenter = true;
	map_spot->m_sDescText.SetText("It's you.");
	map_spot->m_sNameText.SetText("You");

/*	map_spot->GetUIStaticItem().SetOriginalRect(
						pActor->GetMapIconX()*ICON_GRID_WIDTH,
						pActor->GetMapIconY()*ICON_GRID_HEIGHT,
						(pActor->GetMapIconX()+1)*ICON_GRID_WIDTH,
						(pActor->GetMapIconY()+1)*ICON_GRID_HEIGHT);*/

	UIMapBackground.m_vMapSpots.push_back(map_spot);
	UIMapBackground.AttachChild(map_spot);
	m_pActorSpot = map_spot;

	//���������� � ��������� �������
	UIStaticInfo.Show(false);
	UICharacterInfo.Show(false);
}

void CUIMapWnd::AddObjectSpot(CGameObject* pGameObject)
{	
	//!!! ���� ������ ��� CEntity
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

	if (&UIMapBgndV == pWnd)
	{
		if (CUIScrollBar::VSCROLL == msg)
		{
			static u16 prevScrollPos = 0;
			UIMapBackground.MoveMap(0, -VSCROLLBAR_STEP * (UIMapBgndV.GetScrollPos() - prevScrollPos));
			prevScrollPos = UIMapBgndV.GetScrollPos();
		}
	}

	if (&UIMapBgndH == pWnd)
	{
		if (CUIScrollBar::HSCROLL == msg)
		{
			static u16 prevScrollPos = 0;
			UIMapBackground.MoveMap(-HSCROLLBAR_STEP * (UIMapBgndV.GetScrollPos() - prevScrollPos), 0);
			prevScrollPos = UIMapBgndH.GetScrollPos();
		}
	}

	if (&UIMapBackground == pWnd && CUIMapBackground::MAP_MOVED == msg)
	{
		Fvector2	dest;
		UIMapBackground.ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);
		if (UIMapBgndV.IsShown())
			UIMapBgndV.SetScrollPos(static_cast<s16>(UIMapBgndV.GetMaxRange() - iCeil((UIMapBackground.m_fMapHeightMeters - UIMapBackground.m_fMapViewHeightMeters - UIMapBackground.m_fMapY) / dest.y)));
		if (UIMapBgndH.IsShown())
			UIMapBgndH.SetScrollPos(static_cast<s16>(UIMapBgndH.GetMaxRange() - iCeil((UIMapBackground.m_fMapWidthMeters - UIMapBackground.m_fMapViewWidthMeters - UIMapBackground.m_fMapX) / dest.x)));
	}
	inherited::SendMessage(pWnd, msg, pData);
}


void CUIMapWnd::Update()
{
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	m_pActorSpot->m_fHeading = h;

	inherited::Update();
}

void CUIMapWnd::Draw()
{
	inherited::Draw();
}

//������� �� ������� ������� ��������� � ���������� �����
//� ������� ��������� � ����� ������� ����
void CUIMapWnd::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = iFloor(0.5f + m_iMapWidth*(src.x - m_fWorldMapLeft)/m_fWorldMapWidth);
	dest.y = m_iMapHeight - iFloor(0.5f + m_iMapHeight*(src.z - m_fWorldMapTop)/m_fWorldMapHeight);
}

//���������� ����� � �������� �� ���
void CUIMapWnd::DrawMap()
{
}

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	UIMapBackground.SetActivePos(vNewPoint);
	UIMapBackground.m_bNoActorFocus = true;
}
