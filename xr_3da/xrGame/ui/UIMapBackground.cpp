// CUIMapBackground.cpp:������������ �������� (��� ��������)
//						������������� �����
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIMapBackground.h"

#include "../actor.h"
#include "../level.h"
#include "../levelFogOfWar.h"


//������ ���������������� � ������� ������
//���� ����� � ������
//#define MAP_VIEW_WIDTH_METERS 100.f
//#define MAP_VIEW_HEIGHT_METERS 100.f

//������� �������� ������������� ������ �����
#define MAP_PIXEL_TO_METERS ((float)300.f/Device.dwWidth)
#define FOG_OF_WAR_TEXTURE "ui\\ui_fog_of_war"
#define FOG_TEX_WIDTH		64
#define FOG_TEX_HEIGHT		64
#define FOG_TEX_SIZE		FOG_TEX_WIDTH


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


	m_fMapViewWidthMeters =  width*MAP_PIXEL_TO_METERS;
	m_fMapViewHeightMeters = height*MAP_PIXEL_TO_METERS;

	m_iMapViewWidthPixels = width;
	m_iMapViewHeightPixels = height;



	
	//��������� ������ ����� �������� ������ � ��������
	Fvector cell_world_pos;
	Ivector2 screen_pos, screen_pos1;
	Level().m_pFogOfWar->GetFogCellWorldPos(0,0,cell_world_pos);
	ConvertToLocal(cell_world_pos, screen_pos);
	Level().m_pFogOfWar->GetFogCellWorldPos(1,1,cell_world_pos);
	ConvertToLocal(cell_world_pos, screen_pos1);
	
	int fog_cell_size = _max(_abs(screen_pos1.x - screen_pos.x),
							 _abs(screen_pos1.y - screen_pos.y));

	AttachChild(&m_fogOfWarCell);
	m_fogOfWarCell.Init(FOG_OF_WAR_TEXTURE, 0, 0, fog_cell_size, fog_cell_size);
	m_fogOfWarCell.Show(false);
	m_fogOfWarCell.Enable(false);
	
	m_fogOfWarCell.SetTextureScale((float)fog_cell_size/(FOG_TEX_SIZE-2));
}

//���������� ������ ��� ����� ������� �����
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

//������� �� ������� ������� ��������� � ���������� ����� �� ������
//� ������� ��������� � ����� ������� ����
void CUIMapBackground::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = iFloor(0.5f + ((src.x - m_fMapLeftMeters - m_fMapX)/m_fMapViewWidthMeters)*(float)m_iMapViewWidthPixels);

	dest.y = iFloor(0.5f + ((m_fMapHeightMeters - (src.z - m_fMapBottomMeters)
			     - m_fMapY)/m_fMapViewHeightMeters)*(float)m_iMapViewHeightPixels);
}

//������������� �������� ����������� �������
void CUIMapBackground::ConvertFromLocalToMap(int x, int y, Fvector2& dest)
{
	dest.x = m_fMapViewWidthMeters*float(x)/float(m_iMapViewWidthPixels);
	dest.y = m_fMapViewHeightMeters*float(y)/float(m_iMapViewHeightPixels);

}

//������� �� ������� ������� ��������� � 
//���������� ���������� �����
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


	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor) Level().m_pFogOfWar->GetFogCell(pActor->Position(), cell_left_top_pos);

	//�������� ������� �������� ������
	//� ����� ������� ������� ����� �����
	Level().m_pFogOfWar->GetFogCell(left_top_pos, cell_left_top_pos);
	//� ������ ������ ������� ����� �����
	Level().m_pFogOfWar->GetFogCell(right_bottom_pos, cell_right_bottom_pos);

	int i = 0, j = 0;

	for(i=cell_left_top_pos.y; i<=cell_right_bottom_pos.y; ++i)
	{
		for(j=cell_left_top_pos.x; j<=cell_right_bottom_pos.x; ++j)
		{
			DrawFogOfWarCell(j, i);
		}
	}
}

void CUIMapBackground::DrawFogOfWarCell(int x, int y)
{
	if(Level().m_pFogOfWar->GetFogCell(x,y).IsOpened()) return;

	unsigned char fog_shape = Level().m_pFogOfWar->GetFogCell(x,y).shape;


	if(fog_shape == 0)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 1, 0*FOG_TEX_HEIGHT + 1,
										2*FOG_TEX_WIDTH - 1, 1*FOG_TEX_HEIGHT - 1);
	}
	else if((fog_shape & FOG_OPEN_LEFT) && (fog_shape & FOG_OPEN_UP))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										3*FOG_TEX_WIDTH + 1, 2*FOG_TEX_HEIGHT + 1,
										4*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if((fog_shape & FOG_OPEN_RIGHT) && (fog_shape & FOG_OPEN_UP))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										2*FOG_TEX_WIDTH + 1, 2*FOG_TEX_HEIGHT + 1,
										3*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if((fog_shape & FOG_OPEN_LEFT) && (fog_shape & FOG_OPEN_DOWN))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										3*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										4*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);

	}
	else if((fog_shape & FOG_OPEN_RIGHT) && (fog_shape & FOG_OPEN_DOWN))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										2*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										3*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_LEFT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										0*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										1*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);
	}	
	else if(fog_shape & FOG_OPEN_RIGHT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										2*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_UP)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										0*FOG_TEX_WIDTH + 1, 2*FOG_TEX_HEIGHT + 1,
										1*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_DOWN)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 0, 2*FOG_TEX_HEIGHT + 0,
										2*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_UP_LEFT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										2*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										3*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_UP_RIGHT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										3*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										4*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_DOWN_LEFT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										0*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										1*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_DOWN_RIGHT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										2*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else
	{
		m_fogOfWarCell.SetText("?");
	}

	Fvector cell_world_pos;
	Ivector2 screen_pos;

	
	Level().m_pFogOfWar->GetFogCellWorldPos(x,y,cell_world_pos);
	ConvertToLocal(cell_world_pos, screen_pos);
	m_fogOfWarCell.MoveWindow(screen_pos.x, screen_pos.y);
	m_fogOfWarCell.Update();
	m_fogOfWarCell.Draw();
}

void CUIMapBackground::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	
	int deltaX = 0;
	int deltaY = 0;


	//��������� �������� �� ������ �� ������
	//���������� ������ ������������ ����� ������
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
			
			//���������� ����
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

	//��������� ��������� ����
	m_iOldMouseX = x;
	m_iOldMouseY = y;
}

//���������� ������ �� �����
void CUIMapBackground::UpdateMapSpots()
{
	//���������� �� ����� �������
	Ivector2 pos;
	
	for(u32 i = 0; i<m_vMapSpots.size(); ++i)
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

//���������� ��������� �����, ��� ����� ����� 
//��� �� ������
void CUIMapBackground::UpdateActorPos()
{
	//���������� ��������� �����, ��� ����� ����� 
	//��� �� ������
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