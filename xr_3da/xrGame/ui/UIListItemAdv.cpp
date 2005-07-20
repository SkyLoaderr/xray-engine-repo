#include "stdafx.h"
#include "UIListItemAdv.h"

CUIListItemAdv::~CUIListItemAdv(){
	for (my_it it = m_fields.begin(); it != m_fields.end(); it++){
		xr_delete(*it);
	}
}
void CUIListItemAdv::AddField(LPCSTR val, float width){
	float height = GetHeight();

	CUIStatic* st = xr_new<CUIStatic>();
	st->Init(GetNextLeftPos(), 0, width, height);
	st->SetText(val);
	AttachChild(st);

	m_fields.push_back(st);

}

float CUIListItemAdv::GetNextLeftPos(){
	float p = 0;
	for (my_it it = m_fields.begin(); it != m_fields.end(); it++){
		p += (*it)->GetWidth();
	}

	return p;
}