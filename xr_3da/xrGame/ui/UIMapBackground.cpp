// CUIMapBackground.cpp:перемещаемая картинка (сам ландшафт)
//						интреактивной карты
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIMapBackground.h"

#include "..\\actor.h"
#include "..\\level.h"
#include "..\\levelFogOfWar.h"


//размер просматриваемого в текущей момент
//окна карты в метрах
#define MAP_VIEW_WIDTH_METERS 100.f
#define MAP_VIEW_HEIGHT_METERS 100.f


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapBackground::CUIMapBackground()
{
	Show(false);
	Enable(false);
}

CUIMapBackground::~CUIMapBackground()
{
}


void CUIMapBackground::Init(int x, int y, int width, int height)
{
	CUIWindow::Init(x, y, width, height);

	RECT rect = GetAbsoluteRect();
	
	landscape.Init("ui\\ui_minimap_level3",	"hud\\default",	rect.left, rect.top, alNone);
	landscape.SetRect(0, 0, width, height);

	fog_of_war.Init("ui\\ui_button_02",	"hud\\default",	0, 0, alNone);

	AttachChild(&m_fogOfWarCell);
	m_fogOfWarCell.Init("ui\\ui_button_02", 0, 0, 150,40);
	m_fogOfWarCell.Show(false);
	m_fogOfWarCell.Enable(false);


	m_fMapViewWidthMeters =  MAP_VIEW_WIDTH_METERS;
	m_fMapViewHeightMeters = MAP_VIEW_HEIGHT_METERS;

	m_iMapViewWidthPixels = width;
	m_iMapViewHeightPixels = height;

}

//вызывается каждый раз перед вызовом карты
void CUIMapBackground::InitMapBackground()
{
	//Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
	Fbox level_box;
	level_box.x2 = 359.843f;
	level_box.x1 = -280.157f;
	level_box.z2 = 253.36f;
	level_box.z1 = -386.64f;

	m_fMapWidthMeters = level_box.x2 - level_box.x1;
	m_fMapHeightMeters = level_box.z2 - level_box.z1;
	m_fMapLeftMeters = level_box.x1;
	m_fMapTopMeters = level_box.z2;
	m_fMapBottomMeters = level_box.z1;

	UpdateActorPos();
	UpdateMapSpots();
}

/*
void CUIMapBackground::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	inherited::SendMessage(pWnd, msg, pData);
}*/



//перевод из мировой системы координат в координаты карты на экране
//с началом координат в левом верхнем углу
void CUIMapBackground::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = int(m_iMapViewWidthPixels*
				(src.x - m_fMapLeftMeters - m_fMapX)/m_fMapViewWidthMeters);

	dest.y = int(m_iMapViewHeightPixels*
				(m_fMapHeightMeters - (src.z - m_fMapBottomMeters)
			     - m_fMapY)/m_fMapViewHeightMeters);
}

//сопоставление экранным координатам мировых
void CUIMapBackground::ConvertFromLocalToMap(int x, int y, Fvector2& dest)
{
	dest.x = m_fMapViewWidthMeters*float(x)/float(m_iMapViewWidthPixels);
	dest.y = m_fMapViewHeightMeters*float(y)/float(m_iMapViewHeightPixels);

}

//перевод из мировой системы координат в 
//текстурные координаты карты
void CUIMapBackground::ConvertToTexture(const Fvector& src, Fvector2& dest)
{
	ConvertToTexture(src.x, src.z, dest);
}

void CUIMapBackground::ConvertToTexture(float x, float y, Fvector2& dest)
{
	dest.x = x/m_fMapWidthMeters;
	dest.y = y/m_fMapHeightMeters;
}

void CUIMapBackground::Update()
{
	UpdateMapSpots();


	//ignore CUIButton update, call CUIWindow not inherited::Update()
	CUIWindow::Update();
}

void CUIMapBackground::Draw()
{
	Fvector2 top_left;
	Fvector2 right_bottom;
	
	ConvertToTexture(m_fMapX, m_fMapY, top_left);
	ConvertToTexture(m_fMapX + m_fMapViewWidthMeters,
					 m_fMapY + m_fMapViewHeightMeters, 
					 right_bottom);
	
	landscape.Render( top_left.x,	top_left.y, 
				      top_left.x,	right_bottom.y,
				  right_bottom.x,	right_bottom.y,
				  right_bottom.x,	top_left.y);

	//ignore CUIButton draw
	CUIWindow::Draw();

	DrawFogOfWar();
}

void CUIMapBackground::DrawFogOfWar()
{
	Ivector2 cell_left_top_pos;
	Ivector2 cell_right_bottom_pos;
	
	Fvector2 left_top_pos;
	Fvector2 right_bottom_pos;
	
	left_top_pos.x = m_fMapX + m_fMapLeftMeters;
	left_top_pos.y = m_fMapHeightMeters - m_fMapY + m_fMapBottomMeters - m_fMapViewHeightMeters;

	right_bottom_pos.x = left_top_pos.x + m_fMapViewWidthMeters;
	right_bottom_pos.y = left_top_pos.y + m_fMapViewHeightMeters;


	CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(l_pA) Level().m_pFogOfWar->GetFogCell(l_pA->Position(), cell_left_top_pos);

	//получить индексы квадрата тумана
	//в левой верхней видимой части карты
	Level().m_pFogOfWar->GetFogCell(left_top_pos, cell_left_top_pos);
	//в правой нижней видимой части карты
	Level().m_pFogOfWar->GetFogCell(right_bottom_pos, cell_right_bottom_pos);

	int i = 0, j = 0;

	for(i=cell_left_top_pos.y; i<=cell_right_bottom_pos.y; i++)
	{
		for(j=cell_left_top_pos.x; j<=cell_right_bottom_pos.x; j++)
		{
			DrawFogOfWarCell(j, i);
		}
	}
}

void CUIMapBackground::DrawFogOfWarCell(int x, int y)
{
	if(Level().m_pFogOfWar->GetFogCell(x,y).opened) return;

	Fvector cell_world_pos;
	Ivector2 screen_pos;
				
	Level().m_pFogOfWar->GetFogCellWorldPos(x,y,cell_world_pos);
	ConvertToLocal(cell_world_pos, screen_pos);

	switch(Level().m_pFogOfWar->GetFogCell(x,y).shape)
	{
	case 0:
		m_fogOfWarCell.SetText("");
		break;
	case FOG_OPEN_LEFT:
		m_fogOfWarCell.SetText("L");
		break;
	case FOG_OPEN_RIGHT:
		m_fogOfWarCell.SetText("R");
		break;
	case FOG_OPEN_UP:
		m_fogOfWarCell.SetText("U");
		break;
	case FOG_OPEN_DOWN:
		m_fogOfWarCell.SetText("D");
		break;
	case FOG_OPEN_LEFT | FOG_OPEN_UP:
		m_fogOfWarCell.SetText("LU");
		break;
	case FOG_OPEN_RIGHT | FOG_OPEN_UP:
		m_fogOfWarCell.SetText("RU");
		break;
	case FOG_OPEN_LEFT | FOG_OPEN_DOWN:
		m_fogOfWarCell.SetText("LD");
		break;
	case FOG_OPEN_RIGHT | FOG_OPEN_DOWN:
		m_fogOfWarCell.SetText("RD");
		break;
	case FOG_OPEN_RIGHT | FOG_OPEN_DOWN | FOG_OPEN_LEFT:
		m_fogOfWarCell.SetText("RDL");
		break;
	case FOG_OPEN_RIGHT | FOG_OPEN_UP | FOG_OPEN_LEFT:
		m_fogOfWarCell.SetText("RUL");
		break;
	case FOG_OPEN_UP | FOG_OPEN_DOWN | FOG_OPEN_LEFT:
		m_fogOfWarCell.SetText("UDL");
		break;
	case FOG_OPEN_UP | FOG_OPEN_DOWN | FOG_OPEN_RIGHT:
		m_fogOfWarCell.SetText("UDR");
		break;
	case FOG_OPEN_UP | FOG_OPEN_DOWN | FOG_OPEN_RIGHT | FOG_OPEN_LEFT:
		m_fogOfWarCell.SetText("UDLR");
		break;
	case FOG_OPEN_UP | FOG_OPEN_DOWN:
		m_fogOfWarCell.SetText("UD");
		break;
	case FOG_OPEN_RIGHT | FOG_OPEN_LEFT:
		m_fogOfWarCell.SetText("LR");
		break;
	default:
		m_fogOfWarCell.SetText("?");
		break;
	};

	m_fogOfWarCell.MoveWindow(screen_pos.x, screen_pos.y);
	m_fogOfWarCell.Update();
	m_fogOfWarCell.Draw();

}

void CUIMapBackground::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	
	int deltaX = 0;
	int deltaY = 0;


	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
	{
		cursor_on_button = true;
	}
	else
	{
		cursor_on_button = false;
	}

	
	m_bCursorOverButton = cursor_on_button;


	m_bButtonClicked = false;


	if(mouse_action == MOUSE_MOVE && m_eButtonState == BUTTON_NORMAL)
			GetParent()->SetCapture(this, cursor_on_button);

	if(mouse_action == LBUTTON_DB_CLICK && m_bCursorOverButton)
	{
		GetParent()->SetCapture(this, false);
	}
	else if(mouse_action == RBUTTON_DOWN && m_bCursorOverButton)
	{
//		GetTop()->SendMessage(this, ITEM_RBUTTON_CLICK);
	}


	if(m_eButtonState == BUTTON_NORMAL)
	{
		if(mouse_action == LBUTTON_DOWN)
		{
			m_eButtonState = BUTTON_PUSHED;
			
			GetParent()->SetCapture(this, true);
			m_previousPos.x = GetWndRect().left;
			m_previousPos.y = GetWndRect().top;
		}
	}
	else if(m_eButtonState == BUTTON_PUSHED)
	{
		if(mouse_action == LBUTTON_UP)
		{
			if(cursor_on_button)
			{
				m_bButtonClicked = true;
			}
		
			m_eButtonState = BUTTON_NORMAL;
			
			//освободить мышь
			GetParent()->SetCapture(this, false);
		}
		else if(mouse_action == MOUSE_MOVE)
		{
			deltaX = x - m_iOldMouseX;
			deltaY = y - m_iOldMouseY;
				
			Fvector2 dest;
			ConvertFromLocalToMap(deltaX, deltaY, dest);

			m_fMapX -= dest.x;
			m_fMapY -= dest.y;

			if(m_fMapX<0) m_fMapX = 0;
			if(m_fMapX>m_fMapWidthMeters - m_fMapViewWidthMeters) 
						m_fMapX = m_fMapWidthMeters - m_fMapViewWidthMeters;

			if(m_fMapY<0) m_fMapY = 0;
			if(m_fMapY>m_fMapHeightMeters - m_fMapViewHeightMeters) 
						m_fMapY = m_fMapHeightMeters - m_fMapViewHeightMeters;

		}
	}
	else if(m_eButtonState == BUTTON_UP)
	{
		if(mouse_action == MOUSE_MOVE)
		{
			if(cursor_on_button)
				m_eButtonState = BUTTON_PUSHED;
		}
		else if(mouse_action == LBUTTON_UP)
		{
			m_eButtonState = BUTTON_NORMAL;
			GetParent()->SetCapture(this, false);
		}
	}

	//запомнить положение мыши
	m_iOldMouseX = x;
	m_iOldMouseY = y;
}

//обновления иконок на карте
void CUIMapBackground::UpdateMapSpots()
{
	//разместить на карте объекты
	Ivector2 pos;
	
	for(u32 i = 0; i<m_vMapSpots.size(); i++)
	{
		if(m_vMapSpots[i]->m_pObject)
		{
			ConvertToLocal(m_vMapSpots[i]->m_pObject->Position(), pos);
		}
		else
		{
			ConvertToLocal(m_vMapSpots[i]->m_vWorldPos, pos);
		}

		m_vMapSpots[i]->MoveWindow(pos.x, pos.y);
	}
}

//установить положение карты, так чтобы актер 
//был по центру
void CUIMapBackground::UpdateActorPos()
{
	//установить положение карты, так чтобы актер 
	//был по центру
	CEntityAlive *pActor = dynamic_cast<CEntityAlive*>(Level().CurrentEntity());
	if(!pActor) return;

	m_fMapX = pActor->Position().x - m_fMapLeftMeters 
					- m_fMapViewWidthMeters/2.f;
	m_fMapY = m_fMapHeightMeters - (pActor->Position().z - m_fMapBottomMeters)
					- m_fMapViewHeightMeters/2.f;
	
	if(m_fMapX<0) m_fMapX = 0;
	if(m_fMapX>m_fMapWidthMeters - m_fMapViewWidthMeters) 
						m_fMapX = m_fMapWidthMeters - m_fMapViewWidthMeters;

	if(m_fMapY<0) m_fMapY = 0;
	if(m_fMapY>m_fMapHeightMeters - m_fMapViewHeightMeters) 
						m_fMapY = m_fMapHeightMeters - m_fMapViewHeightMeters;
}