// CUIMapWnd.h:  диалог итерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "..\\HUDManager.h"

#include "..\\actor.h"
#include "..\\level.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"



#define MAP_LEFT		89
#define MAP_TOP			93
#define MAP_RADIUS		80



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapWnd::CUIMapWnd()
{
	Hide();
	
	SetFont(HUD().pFontHeaderRussian);


	m_iMapWidth = 600;
	m_iMapHeight = 450;

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
	for(u32 i=0; i<UIMapBackground.m_vMapSpots.size(); i++)
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


	AttachChild(&UIMainMapFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainMapFrame);

	UIMainMapFrame.AttachChild(&UICheckButton1);
	xml_init.InitButton(uiXml, "button", 0, &UICheckButton1);
	UIMainMapFrame.AttachChild(&UICheckButton2);
	xml_init.InitButton(uiXml, "button", 1, &UICheckButton2);
	UIMainMapFrame.AttachChild(&UICheckButton3);
	xml_init.InitButton(uiXml, "button", 2, &UICheckButton3);
	UIMainMapFrame.AttachChild(&UICheckButton4);
	xml_init.InitButton(uiXml, "button", 3, &UICheckButton4);

	//информация о точке на карте на которую наводит игрок
	UIMainMapFrame.AttachChild(&UIStaticInfo);
	UIStaticInfo.Init(10,10,150,60);
	UIStaticInfo.SetText("info");
	
	UIMainMapFrame.AttachChild(&UIMapBackground);
	UIMapBackground.Init(10,10,m_iMapWidth,m_iMapHeight);
	UIMapBackground.Show(true);
	UIMapBackground.Enable(true);
}

void CUIMapWnd::InitMap()
{
	//Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
	Fbox level_box;
	level_box.x2 = 359.843f;
	level_box.x1 = -280.157f;
	level_box.z2 = 253.36f;
	level_box.z1 = -386.64f;

	m_fWorldMapWidth = level_box.x2 - level_box.x1;
	m_fWorldMapHeight = level_box.z2 - level_box.z1;

	m_fWorldMapLeft = level_box.x1;
	m_fWorldMapTop = level_box.z1;


	UIMapBackground.InitMapBackground();



	//добавить интерактивные отметки на карте 
	RemoveAllSpots();

	Ivector2 P;
	
	//CActor* m_Actor = dynamic_cast<CActor*>(Level().CurrentEntity());
	CEntityAlive* m_Actor = dynamic_cast<CEntityAlive*>(Level().CurrentEntity());
	
	if (!m_Actor) return;

	CTeam& Team = Level().Teams[m_Actor->id_Team];

	int left = UIMapBackground.GetWndRect().left;
	int top = UIMapBackground.GetWndRect().top;

	for (u32 i=0; i<Team.Squads.size(); i++){
			CSquad& S = Team.Squads[i];
			for (u32 j=0; j<S.Groups.size(); j++){
				CGroup& G = S.Groups[j];
				for (u32 k=0; k<G.Members.size(); k++){
					if (G.Members[k]->IsVisibleForHUD())
					{
					
						UIMapBackground.ConvertToLocal(G.Members[k]->Position(),P);

						CUIMapSpot* map_spot;
						map_spot = xr_new<CUIMapSpot>();
						map_spot->Init("ui\\ui_button_02", P.x + left, 
									   P.y + top, 30,30);

						map_spot->m_pObject = G.Members[k];

						map_spot->SetText("x");
						UIMapBackground.m_vMapSpots.push_back(map_spot);
						UIMapBackground.AttachChild(map_spot);
					}
				}
			}
		}


	{
		Fvector src;
		src.x = -100.f;
		src.y = 0;
		src.z = 25.f;

		UIMapBackground.ConvertToLocal(src,P);
		CUIMapSpot* map_spot;
		map_spot = xr_new<CUIMapSpot>();
		map_spot->Init("ui\\ui_button_02", P.x + left, 
							   P.y + top, 100,100);
		map_spot->m_pObject = NULL;
		map_spot->m_vWorldPos.set(0.f,0.f,0.f);
		UIMapBackground.m_vMapSpots.push_back(map_spot);
		UIMapBackground.AttachChild(map_spot);
	}
}


void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	inherited::SendMessage(pWnd, msg, pData);
}


void CUIMapWnd::Update()
{
	inherited::Update();
}

void CUIMapWnd::Draw()
{
	//landscape.Render();
	
	inherited::Draw();
}

//перевод из мировой системы координат в координаты карты
//с началом координат в левом верхнем углу
void CUIMapWnd::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = int(m_iMapWidth*(src.x - m_fWorldMapLeft)/m_fWorldMapWidth);
	dest.y = m_iMapHeight - int(m_iMapHeight*(src.z - m_fWorldMapTop)/m_fWorldMapHeight);
}

//прорисовка карты и объектов на ней
void CUIMapWnd::DrawMap()
{
}