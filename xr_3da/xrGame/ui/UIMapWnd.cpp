// CUIMapWnd.h:  ������ ������������ �����
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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapWnd::CUIMapWnd()
{
	Hide();
	
	SetFont(HUD().pFontSmall);

	Init();
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


	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);


	AttachChild(&UIMainMapFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainMapFrame);
	UIMainMapFrame.AttachChild(&UIPDAHeader);
	xml_init.InitStatic(uiXml, "static", 0, &UIPDAHeader);


	UIMainMapFrame.AttachChild(&UICheckButton1);
	xml_init.InitButton(uiXml, "button", 0, &UICheckButton1);
	UIMainMapFrame.AttachChild(&UICheckButton2);
	xml_init.InitButton(uiXml, "button", 1, &UICheckButton2);
	UIMainMapFrame.AttachChild(&UICheckButton3);
	xml_init.InitButton(uiXml, "button", 2, &UICheckButton3);
	UIMainMapFrame.AttachChild(&UICheckButton4);
	xml_init.InitButton(uiXml, "button", 3, &UICheckButton4);

	//���������� � ����� �� ����� �� ������� ������� �����
	UIMainMapFrame.AttachChild(&UIStaticInfo);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticInfo);

	UIStaticInfo.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0, UIStaticInfo.GetWidth(), UIStaticInfo.GetHeight(), "map_character.xml");

	
	UIMainMapFrame.AttachChild(&UIMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UIMapBackground);
	UIMapBackground.Show(true);
	UIMapBackground.Enable(true);
	
	m_iMapWidth = UIMapBackground.GetWidth();
	m_iMapHeight = UIMapBackground.GetHeight();
}

void CUIMapWnd::InitMap()
{
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
		UIMapBackground.m_LevelBox.x2 = 359.843f;
		UIMapBackground.m_LevelBox.x1 = -280.157f;
		UIMapBackground.m_LevelBox.z2 = 253.36f;
		UIMapBackground.m_LevelBox.z1 = -386.64f;
	}

	m_fWorldMapWidth = UIMapBackground.m_LevelBox.x2 - UIMapBackground.m_LevelBox.x1;
	m_fWorldMapHeight = UIMapBackground.m_LevelBox.z2 - UIMapBackground.m_LevelBox.z1;

	m_fWorldMapLeft = UIMapBackground.m_LevelBox.x1;
	m_fWorldMapTop = UIMapBackground.m_LevelBox.z1;


	UIMapBackground.InitMapBackground();

	///������ ��� ������������� ������� �� ����� 
	RemoveAllSpots();

	Ivector2 P;
	
	CActor* pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
	if (!pActor) return;

	CTeam& Team = Level().Teams[pActor->id_Team];

	int left = UIMapBackground.GetWndRect().left;
	int top = UIMapBackground.GetWndRect().top;

	for (u32 i=0; i<Team.Squads.size(); ++i)
	{
			CSquad& S = Team.Squads[i];
			for (u32 j=0; j<S.Groups.size(); ++j)
			{
				CGroup& G = S.Groups[j];
				for (u32 k=0; k<G.Members.size(); ++k)
				{
					if (G.Members[k]->IsVisibleForHUD())
					{
						UIMapBackground.ConvertToLocal(G.Members[k]->Position(),P);

						CUIMapSpot* map_spot;
						map_spot = xr_new<CUIMapSpot>();

						map_spot->m_pObject = G.Members[k];
						
						CEntity* pEntity = dynamic_cast<CEntity*>(G.Members[k]);

						map_spot->SetShader(GetCharIconsShader());
						map_spot->SetWidth(MAP_ICON_WIDTH);
						map_spot->SetHeight(MAP_ICON_HEIGHT);
						map_spot->MoveWindow(P.x + left, P.y + top);
						map_spot->m_bCenter = true;

						map_spot->GetUIStaticItem().SetOriginalRect(
											pEntity->GetMapIconX()*ICON_GRID_WIDTH,
											pEntity->GetMapIconY()*ICON_GRID_HEIGHT,
											(pEntity->GetMapIconX()+1)*ICON_GRID_WIDTH,
											(pEntity->GetMapIconY()+1)*ICON_GRID_HEIGHT);
						//if(Level().CurrentEntity() == G.Members[k]) map_spot->SetText("we");
						UIMapBackground.m_vMapSpots.push_back(map_spot);
						UIMapBackground.AttachChild(map_spot);
					}
				}
			}
		}

	////////////////////////////////////////////
	//�������� �������, ��������� � ������ PDA
	////////////////////////////////////////////
	
	for(KNOWN_INFO_PAIR_IT it = pActor->GetPDA()->m_mapKnownInfo.begin();
		pActor->GetPDA()->m_mapKnownInfo.end() != it;
		++it)
	{
		//���������� ������� ���������� � ������� �� ��������
		CInfoPortion info_portion;
		info_portion.Load((*it).first);

		if(!info_portion.LocationExist()) continue;
		SMapLocation& map_location = info_portion.GetLocation();
		
		Fvector src;
		src.x = map_location.x;
		src.y = 0;
		src.z = map_location.y;

		UIMapBackground.ConvertToLocal(src,P);
		CUIMapSpot* map_spot;
		map_spot = xr_new<CUIMapSpot>();
		
		map_spot->SetShader(GetCharIconsShader());
		map_spot->MoveWindow(P.x + left, P.y + top);
		map_spot->SetWidth(MAP_ICON_WIDTH);
		map_spot->SetHeight(MAP_ICON_HEIGHT);
		map_spot->GetUIStaticItem().SetOriginalRect(
						map_location.icon_x*ICON_GRID_WIDTH,
						map_location.icon_y*ICON_GRID_HEIGHT,
						map_location.icon_width*ICON_GRID_WIDTH,
						map_location.icon_height*ICON_GRID_HEIGHT);
		
		map_spot->m_pObject = NULL;
		map_spot->m_vWorldPos.set(map_location.x,0.f,map_location.y);
		UIMapBackground.m_vMapSpots.push_back(map_spot);
		UIMapBackground.AttachChild(map_spot);
	}


	//���������� � ��������� �������
	UIStaticInfo.Show(false);
	UICharacterInfo.Show(false);
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
				if(pInvOwner)
				{
					UICharacterInfo.InitCharacter(pInvOwner);
					UICharacterInfo.Show(true);
				}
				else
					UICharacterInfo.Show(false);
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