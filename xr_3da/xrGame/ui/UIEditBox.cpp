// CUIEditBox.cpp: ввод строки с клавиатуры
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uieditbox.h"


static u32 DILetters[] = { DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, 
						   DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, 
						   DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, 
						   DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, 
						   DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z};

static xr_map<u32, char> gs_DIK2CHR;


CUIEditBox::CUIEditBox(void)
{
	for(char l_c = 'a'; l_c <= 'z'; l_c++) gs_DIK2CHR[DILetters[l_c-'a']] = l_c;

	m_bShift = false;
	m_bInputFocus = false;
}

CUIEditBox::~CUIEditBox(void)
{
}

void CUIEditBox::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	if(mouse_action == LBUTTON_DOWN)
	{
		GetParent()->SetKeyboardCapture(this, true);
		m_bInputFocus = true;
	}
}


bool CUIEditBox::OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action)
{	
	if(!m_bInputFocus) return false;


	xr_map<u32, char>::iterator it;
	bool str_updated = false; 
		

	if(keyboard_action == KEY_PRESSED)	
	{

		switch(dik)
		{
		case DIK_LSHIFT:
		case DIK_RSHIFT:
			m_bShift = true;
			break;
		case DIK_RETURN:
		case DIK_NUMPADENTER:
			GetParent()->SetKeyboardCapture(this, false);
			m_bInputFocus = false;
			return true;
			break;

		case DIK_BACKSPACE:
			if(m_sEdit.size()>=2)
			{
				m_sEdit.pop_back();
				m_sEdit.pop_back();
				m_sEdit.push_back(0);
			}

			str_updated = true; 
			break;

		case DIK_SPACE:
			AddChar(' ');
			str_updated = true; 
			break;

		default:
			it = gs_DIK2CHR.find(dik);
			
			//нажата клавиша с буквой 
			if(it != gs_DIK2CHR.end())
			{
				AddLetter(gs_DIK2CHR[dik]);
				str_updated = true; 
			}
			break;
		}

		if(str_updated)
		{
			m_str = &m_sEdit[0];
			return true;
		}
	}
	else if(keyboard_action == KEY_RELEASED)	
	{
		switch(dik)
		{
		case DIK_LSHIFT:
		case DIK_RSHIFT:
			m_bShift = false;
		}
	}


	return false;
}

void CUIEditBox::AddChar(char c)
{
	char buf[2];
	buf[0]=c;
	buf[1]=0;
	
	GetFont()->SizeOf(m_str);
/*	
	int text_length;
	text_length = int(GetFont()->SizeOf(m_str) + 
					GetFont()->SizeOf(buf));
	//строка длинее полосы ввода
	if(text_length>GetWidth()) return;*/
	
 

	if(!m_sEdit.empty()) m_sEdit.pop_back();
	m_sEdit.push_back(c);
	m_sEdit.push_back(0);
}

void CUIEditBox::AddLetter(char c)
{
	if(m_bShift)
	{
		c = c-'a';
		c = c+'A';
	}
	
	AddChar(c);
}
