#include "stdafx.h"

#include "UIEditKeyBind.h"
#include "UIColorAnimatorWrapper.h"

#include "xr_level_controller.h"

CUIEditKeyBind::CUIEditKeyBind(){
    m_bEditMode = false;

	m_pAnimation = xr_new<CUIColorAnimatorWrapper>("ui_map_area_anim");
	m_pAnimation->Cyclic(true);
}

void CUIEditKeyBind::SetText(const char* text){
	if (!text || 0 == xr_strlen(text))
		CUILabel::SetText("<Empty>");
	else
		CUILabel::SetText(text);
}

void CUIEditKeyBind::Init(float x, float y, float width, float height){
	CUILabel::Init(x,y,width,height);
	InitTexture("ui_options_string");
}

void CUIEditKeyBind::InitTexture(LPCSTR texture, bool bHorizontal){
	CUILabel::InitTexture(texture,bHorizontal);
}

void CUIEditKeyBind::OnFocusLost(){
	m_bEditMode = false;
	m_lines.SetTextColor((subst_alpha(m_lines.GetTextColor(), color_get_A(0xffffffff))));
}

extern _keybind keynames[];

void CUIEditKeyBind::OnMouseDown(bool left_button){
	if (m_bEditMode){		
		int i = 0;

		if (left_button){
			while (keynames[i].name)
			{
				if (keynames[i].DIK == MOUSE_1)            
				{
					m_val = keynames[i].name;
					SetText(keynames[i].name);
					OnFocusLost();
					return;
				}
				i++;
			}
		}
		else
		{
			while (keynames[i].name)
			{
				if (keynames[i].DIK == MOUSE_2)            
				{
					m_val = keynames[i].name;
					SetText(keynames[i].name);
					OnFocusLost();
					return;
				}
				i++;
			}
		}
	}

	if (left_button)
		m_bEditMode = m_bCursorOverWindow;

	CUILabel::OnMouseDown(left_button);
}

bool CUIEditKeyBind::OnKeyboard(int dik, EUIMessages keyboard_action){
	int i = 0;

	if (CUILabel::OnKeyboard(dik, keyboard_action))
		return true;

	if (m_bEditMode)
		while (keynames[i].name)
		{
            if (keynames[i].DIK == dik)            
			{
				m_val = keynames[i].name;
				string64 buff;
				strcpy(buff, (keynames[i].name[0]=='k')? keynames[i].name+1 : keynames[i].name);
				SetText(buff);
				OnFocusLost();
				return true;
			}
			i++;
		}
	return false;
}

void CUIEditKeyBind::Update(){
	CUILabel::Update();

	m_bTextureAvailable = m_bCursorOverWindow;
	if (m_bEditMode)
	{
		m_pAnimation->Update();
		m_lines.SetTextColor((subst_alpha(m_lines.GetTextColor(), color_get_A(m_pAnimation->GetColor()))));
	}
	
}

void CUIEditKeyBind::SetCurrentValue(){
	string64 buff;
	ZeroMemory(buff,sizeof(buff));
	GetActionBinding(m_entry.c_str(), buff);
	SetText(buff);
}

void CUIEditKeyBind::SaveValue(){
	BindAction2Key(m_val.c_str());
}