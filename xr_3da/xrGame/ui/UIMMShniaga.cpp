#include "StdAfx.h"

#include "UIMMShniaga.h"
#include "UIStatic.h"
#include "UIScrollView.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "../game_base_space.h"
#include "../level.h"
#include <math.h>


CUIMMShniaga::CUIMMShniaga(){
	m_view		= xr_new<CUIScrollView>();	AttachChild(m_view);
	m_shniaga  = xr_new<CUIStatic>();	AttachChild(m_shniaga);
	m_magnifier = xr_new<CUIStatic>();	m_shniaga->AttachChild(m_magnifier);	m_magnifier->SetPPMode();
	m_anims[0] = xr_new<CUIStatic>();	m_shniaga->AttachChild(m_anims[0]);
	m_anims[1] = xr_new<CUIStatic>();	m_shniaga->AttachChild(m_anims[1]);
	m_gratings[0] = xr_new<CUIStatic>();	m_shniaga->AttachChild(m_gratings[0]);
	m_gratings[1] = xr_new<CUIStatic>();	m_shniaga->AttachChild(m_gratings[1]);

	m_mag_pos = 0;

	m_selected = NULL;

	m_start_time = 0;
	m_origin = 0;
	m_destination = 0;
	m_run_time = 0;
}

CUIMMShniaga::~CUIMMShniaga(){
	xr_delete(m_magnifier);
	xr_delete(m_shniaga);
	xr_delete(m_anims[0]);
	xr_delete(m_anims[1]);	
	xr_delete(m_gratings[0]);
	xr_delete(m_gratings[1]);
	xr_delete(m_view);

	for (u32 i = 0; i<m_buttons.size(); i++)
		xr_delete(m_buttons[i]);
	for (u32 i = 0; i<m_buttons_new.size(); i++)
		xr_delete(m_buttons_new[i]);
}

void CUIMMShniaga::Init(CUIXml& xml_doc, LPCSTR path){
	string256 _path;

	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path,path,":shniaga:magnifire"),0,m_magnifier); m_mag_pos = m_magnifier->GetWndPos().x;
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path,path,":shniaga"),0,m_shniaga);
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path,path,":shniaga:left_anim"),0,m_anims[0]);
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path,path,":shniaga:right_anim"),0,m_anims[1]);
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path,path,":shniaga:left_grating"),0,m_gratings[0]);
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path,path,":shniaga:right_grating"),0,m_gratings[1]);
	CUIXmlInit::InitScrollView(xml_doc, strconcat(_path,path,":buttons_region"),0,m_view);

	if (!g_pGameLevel)
	{
		CreateList(m_buttons, xml_doc, "menu_main");
		CreateList(m_buttons_new, xml_doc, "menu_new_game");
	}
	else if (GameID() == GAME_SINGLE)
		CreateList(m_buttons, xml_doc, "menu_main_single");
	else
		CreateList(m_buttons, xml_doc, "menu_main_mm");

	ShowMain();
}

void CUIMMShniaga::CreateList(xr_vector<CUIStatic*>& lst, CUIXml& xml_doc, LPCSTR path){
	CGameFont* pF;
	u32	color;
	float height;

	height = xml_doc.ReadAttribFlt(path, 0, "btn_height");
	R_ASSERT(height);

	CUIXmlInit::InitFont(xml_doc, path, 0, color, pF);
	R_ASSERT(pF);

	int nodes_num	= xml_doc.GetNodesNum(path, 0, "btn");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path,0);
	xml_doc.SetLocalRoot(tab_node);

	CUIStatic* st;

	for (int i = 0; i < nodes_num; ++i)
	{
		st = xr_new<CUIStatic>();
		st->Init(0,0,m_view->GetDesiredChildWidth(), height);
		st->SetTextST(xml_doc.ReadAttrib("btn", i, "caption"));
		if (pF)
			st->SetFont(pF);
		st->SetTextColor(color);
		st->SetTextAlignment(CGameFont::alCenter);
		st->SetTextY(-3);
		st->SetWindowName(xml_doc.ReadAttrib("btn", i, "name"));
		st->SetMessageTarget(this);
		lst.push_back(st);
	}
	xml_doc.SetLocalRoot(xml_doc.GetRoot());

}


void CUIMMShniaga::ShowMain(){
	m_view->Clear();
	for (u32 i = 0; i<m_buttons.size(); i++)
		m_view->AddWindow(m_buttons[i], false);

	SendMessage(m_buttons[0], STATIC_FOCUS_RECEIVED);
}

void CUIMMShniaga::ShowNewGame(){
    m_view->Clear();
	for (u32 i = 0; i<m_buttons_new.size(); i++)
		m_view->AddWindow(m_buttons_new[i], false);

	SendMessage(m_buttons_new[0], STATIC_FOCUS_RECEIVED);
}

bool CUIMMShniaga::IsButton(CUIWindow* st){
	for (u32 i = 0; i<m_buttons.size(); i++)
		if (m_buttons[i] == st)
			return true;

	for (u32 i = 0; i<m_buttons_new.size(); i++)
		if (m_buttons_new[i] == st)
			return true;

	return false;
}

void CUIMMShniaga::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	CUIWindow::SendMessage(pWnd, msg, pData);
	if (IsButton(pWnd)){
		switch (msg){
			case STATIC_FOCUS_RECEIVED:
				m_start_time = Device.TimerAsyncMM();
				m_origin = m_shniaga->GetWndPos().y;
				float border = GetHeight() - m_shniaga->GetHeight();
				float y = pWnd->GetWndPos().y;
				m_destination = (y - m_shniaga->GetHeight()/2 + 3 < border) ? y - m_shniaga->GetHeight()/2 + 3: border;
				m_run_time = u32((log(1 + abs(m_origin - m_destination))/log(GetHeight()))*1000);
				if (m_run_time < 100)
					m_run_time = 100;

				m_selected = pWnd;
				break;
		}

	}
}

void CUIMMShniaga::Update(){
	if (m_start_time > Device.TimerAsyncMM() - 1000)
	{
		Fvector2 pos = m_shniaga->GetWndPos();
		float l = 2*PI*m_anims[0]->GetHeight()/2;
		int n = iFloor(pos.y/l);
		float a = 2*PI*(pos.y - l*n)/l;
		m_anims[0]->SetHeading(-a);
		m_anims[1]->SetHeading(a);

		pos.y = this->pos(m_origin, m_destination, Device.TimerAsyncMM() - m_start_time);
		m_shniaga->SetWndPos(pos);		
	}
	CUIWindow::Update();
}


bool CUIMMShniaga::OnMouse(float x, float y, EUIMessages mouse_action){
	
	Fvector2 pos = UI()->GetUICursor()->GetPos();
    if (WINDOW_LBUTTON_DOWN == mouse_action && m_magnifier->GetAbsoluteRect().in(pos.x, pos.y))
	{
		if (0 == xr_strcmp("btn_new_game", m_selected->WindowName()))
            ShowNewGame();
		else if (0 == xr_strcmp("btn_new_back", m_selected->WindowName()))
            ShowMain();
		else
            GetMessageTarget()->SendMessage(m_selected, BUTTON_CLICKED);
	}

	return CUIWindow::OnMouse(x,y,mouse_action);
}

float CUIMMShniaga::pos(float x1, float x2, u32 t){
	float x = 0;

    if (t>=0 && t<=m_run_time)
        x = log(1 + (t*10.0f)/m_run_time)/log(11.0f);
	else if (t<=0)
		x = 0;
	else if (t>m_run_time)
		x = 1;

	x*=abs(x2 - x1);

	if (x2 - x1 < 0)
		return x1 - x;
	else
        return x1 + x;
}

void CUIMMShniaga::SetVisibleMagnifier(bool f){
	Fvector2 pos = m_magnifier->GetWndPos();
	if (f)
		pos.x = m_mag_pos;
	else
		pos.x = 1025;
	m_magnifier->SetWndPos(pos);

}
