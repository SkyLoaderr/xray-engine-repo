#include "stdafx.h"
#include "UIListItemAdv.h"

CUIListItemAdv::~CUIListItemAdv(){

}

void CUIListItemAdv::SetTextColor(u32 color){
	for (my_it it = m_fields.begin(); it != m_fields.end(); it++)
		(*it)->SetTextColor(color);
}

void CUIListItemAdv::AddField(LPCSTR val, float width){
	float height = GetHeight();

	CUIStatic* st = xr_new<CUIStatic>();
	st->Init(GetNextLeftPos(), 0, width, height);
	st->SetTextComplexMode(false);
	st->SetText(val);
	st->SetAutoDelete(true);
	AttachChild(st);

	m_fields.push_back(st);
}

void CUIListItemAdv::AddWindow(CUIWindow* pWnd){
	Fvector2 pos = pWnd->GetWndPos();
	pos.x = GetNextLeftPos();
	pos.y = (GetHeight() - pWnd->GetHeight())/2;
    pWnd->SetWndPos(pos);
	pWnd->SetAutoDelete(true);
	AttachChild(pWnd);
}

float CUIListItemAdv::GetNextLeftPos(){
	float p = 0;
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
		p += (*it)->GetWidth();

	return p;
}