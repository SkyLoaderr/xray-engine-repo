// CUIEditBox.cpp: ���� ������ � ����������
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <dinput.h>
#include "uieditbox.h"
#include "../HUDManager.h"
#include "xr_ioconsole.h"

static u32 DILetters[] = { DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, 
						   DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, 
						   DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, 
						   DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, 
						   DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z,
						   DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7,
						   DIK_8, DIK_9};

static xr_map<u32, char> gs_DIK2CHR;



CUIEditBox::CUIEditBox(void)
{
	char l_c;
	for(l_c = 'a'; l_c <= 'z'; ++l_c) 
		gs_DIK2CHR[DILetters[l_c-'a']] = l_c;
	for(l_c = '0'; l_c <= '9'; ++l_c)
        gs_DIK2CHR[DILetters['z'-'a'+l_c+1-'0']] = l_c;

	m_bShift = false;
	m_bInputFocus = false;

	m_iKeyPressAndHold = 0;
	m_bHoldWaitMode = false;

	m_iCursorPos = 0;
	SetText("");
}

CUIEditBox::~CUIEditBox(void)
{
}	

void CUIEditBox::SetDefaultValue(){
	SetText(Console->GetString(m_entry.c_str()));
}

void CUIEditBox::SaveValue(){
	xr_string command = m_entry;
	command += " ";
	command += GetText();
	Console->Execute(command.c_str());
}

void CUIEditBox::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == GetParent())
	{
		//���-�� ������ �������� ����������
		if(msg == WINDOW_KEYBOARD_CAPTURE_LOST)
		{
			m_bInputFocus = false;
			m_iKeyPressAndHold = 0;
		}
	}
}

void CUIEditBox::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(mouse_action == WINDOW_LBUTTON_DOWN && !m_bInputFocus)
	{
		GetParent()->SetKeyboardCapture(this, true);
		m_bInputFocus = true;
		m_iKeyPressAndHold = 0;

		m_iCursorPos = xr_strlen(m_lines.GetText());
	}
}


bool CUIEditBox::OnKeyboard(int dik, EUIMessages keyboard_action)
{	
	if(!m_bInputFocus) return false;


	if(keyboard_action == WINDOW_KEY_PRESSED)	
	{
		m_iKeyPressAndHold = dik;
		m_bHoldWaitMode = true;

		if(KeyPressed(dik))	return true;
	}
	else if(keyboard_action == WINDOW_KEY_RELEASED)	
	{
		if(m_iKeyPressAndHold == dik)
		{
			m_iKeyPressAndHold = 0;
			m_bHoldWaitMode = false;
		}

		if(KeyReleased(dik)) return true;
	}


	return false;
}

bool CUIEditBox::KeyPressed(int dik)
{
	xr_map<u32, char>::iterator it;

	char out_me = ' ';

	switch(dik)
	{
	//����������� �������
	case DIK_LEFT:
	case DIKEYBOARD_LEFT:
		if(m_iCursorPos > 0) 
			--m_iCursorPos;
		return true;
	case DIK_RIGHT:
	case DIKEYBOARD_RIGHT:
		if(m_iCursorPos < xr_strlen(m_lines.GetText())) 
			++m_iCursorPos;
		return true;
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		m_bShift = true;
		return true;
	case DIK_ESCAPE:
		SetText("");
		return true;
	case DIK_RETURN:
	case DIK_NUMPADENTER:
		GetParent()->SetKeyboardCapture(this, false);
		m_bInputFocus = false;
		m_iKeyPressAndHold = 0;
		return true;
	case DIK_BACKSPACE:
		if(m_iCursorPos > 0)
		{
			--m_iCursorPos;
			m_lines.DelChar(m_iCursorPos);
		}
		return true;
	case DIK_DELETE:
	case DIKEYBOARD_DELETE:
		if(m_iCursorPos < xr_strlen(m_lines.GetText()))
			m_lines.DelChar(m_iCursorPos);
		return true;
	case DIK_SPACE:
		out_me = ' ';					break;
	case DIK_LBRACKET:
        out_me = m_bShift ? '{' : '[';	break;
	case DIK_RBRACKET:
		out_me = m_bShift ? '}' : ']';	break;
	case DIK_SEMICOLON:
		out_me = m_bShift ? ':' : ';';	break;
	case DIK_APOSTROPHE:
		out_me = m_bShift ? '"' : '\'';	break;
	case DIK_BACKSLASH:
		out_me = m_bShift ? '|' : '\\';	break;
	case DIK_SLASH:
		out_me = m_bShift ? '?' : '/';	break;
	case DIK_COMMA:
		out_me = m_bShift ? '<' : ',';	break;
	case DIK_PERIOD:
		out_me = m_bShift ? '>' : '.';	break;
	case DIK_MINUS:
		out_me = m_bShift ? '_' : '-';	break;
	case DIK_EQUALS:
		out_me = m_bShift ? '+' : '=';	break;
	default:
		it = gs_DIK2CHR.find(dik);
			
		//������ ������� � ������ 
		if (gs_DIK2CHR.end() != it)
			AddLetter((*it).second);

		return true;
	}

	AddChar(out_me);

	return true;
}

bool CUIEditBox::KeyReleased(int dik)
{
	switch(dik)
	{
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		m_bShift = false;
		return true;
	}

	return true;
}



void CUIEditBox::AddChar(char c)
{
	char buf[2];
	buf[0]=c;
	buf[1]=0;
	
	
	int text_length;
	text_length = (int)m_lines.GetFont()->SizeOf(m_lines.GetText());
	
	//������ ������ ������ �����
	if(text_length>GetWidth() - 1) 
		return;

	m_lines.AddChar(c, m_iCursorPos);
	++m_iCursorPos;
}

void CUIEditBox::AddLetter(char c)
{
	if(m_bShift)
	{
		switch(c) {
		case '1': c='!';	break;
		case '2': c='@';	break;
		case '3': c='#';	break;
		case '4': c='$';	break;
		case '5': c='%';	break;
		case '6': c='^';	break;
		case '7': c='&';	break;
		case '8': c='*';	break;
		case '9': c='(';	break;
		case '0': c=')';	break;
		default:
			c = c-'a';
			c = c+'A';
		}
	}
	
	AddChar(c);
}

//����� ��� ������������� ���������
//������� ��� ������������ ������
#define HOLD_WAIT_TIME 400
#define HOLD_REPEAT_TIME 100

void CUIEditBox::Update()
{
	if(m_bInputFocus)
	{	
		static u32 last_time; 

		u32 cur_time = Device.TimerAsync();

		if(m_iKeyPressAndHold)
		{
			if(m_bHoldWaitMode)
			{
				if(cur_time - last_time>HOLD_WAIT_TIME)
				{
					m_bHoldWaitMode = false;
					last_time = cur_time;
				}
			}
			else
			{
				if(cur_time - last_time>HOLD_REPEAT_TIME)
				{
					last_time = cur_time;
					KeyPressed(m_iKeyPressAndHold);
				}
			}
		}
		else
			last_time = cur_time;
	}
	CUILabel::Update();
}

void  CUIEditBox::Draw()
{
	CUILabel::Draw();

	if(m_bInputFocus)
	{	
		//���������� ������
		Frect rect = GetAbsoluteRect();
		float outX, outY;

#pragma todo("Satan->Satan: seems i must to optimize this bullshit")
		xr_string tmp_str = m_lines.GetText();
		tmp_str.assign(tmp_str.begin(), tmp_str.begin()+m_iCursorPos);

		outX = GetFont()->SizeOf(tmp_str.c_str());
		outY = 0;


		GetFont()->SetColor(0xAAFFFF00);
		UI()->OutText(GetFont(), GetWndRect(), (float)rect.left+outX, 
					   (float)rect.top+outY,  "|");
		GetFont()->OnRender();
	}	
}

void CUIEditBox::SetText(LPCSTR str)
{
	CUILabel::SetText(str);
	m_iCursorPos = xr_strlen(m_lines.GetText());
}