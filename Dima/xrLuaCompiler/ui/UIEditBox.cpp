// CUIEditBox.cpp: ���� ������ � ����������
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

	m_iKeyPressAndHold = 0;
	m_bHoldWaitMode = false;

	m_iCursorPos = 0;

	m_sEdit.push_back('y');
	m_sEdit.push_back('u');
	m_sEdit.push_back('r');
	m_sEdit.push_back('i');
	m_sEdit.push_back(0);
}

CUIEditBox::~CUIEditBox(void)
{
}

void CUIEditBox::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == GetParent())
	{
		//���-�� ������ �������� ����������
		if(msg == CUIWindow::KEYBOARD_CAPTURE_LOST)
		{
			m_bInputFocus = false;
			m_iKeyPressAndHold = 0;
		}
	}
}

void CUIEditBox::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	if(mouse_action == LBUTTON_DOWN && !m_bInputFocus)
	{
		GetParent()->SetKeyboardCapture(this, true);
		m_bInputFocus = true;
		m_iKeyPressAndHold = 0;

		//������ � ����� ������, ����� ��������
		//��������� ������
		m_iCursorPos = m_sEdit.size()-1;
	}
}


bool CUIEditBox::OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action)
{	
	if(!m_bInputFocus) return false;


	if(keyboard_action == KEY_PRESSED)	
	{
		m_iKeyPressAndHold = dik;
		m_bHoldWaitMode = true;

		if(KeyPressed(dik))	return true;
	}
	else if(keyboard_action == KEY_RELEASED)	
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
	bool str_updated = false; 


	switch(dik)
	{
	//����������� �������
	case DIK_LEFT:
	case DIKEYBOARD_LEFT:
		if(m_iCursorPos > 0) m_iCursorPos--;
		return true;
		break;
	case DIK_RIGHT:
	case DIKEYBOARD_RIGHT:
		if(m_iCursorPos < m_sEdit.size()-1) m_iCursorPos++;
		return true;
		break;
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		m_bShift = true;
		return true;
		break;
	case DIK_RETURN:
	case DIK_NUMPADENTER:
		GetParent()->SetKeyboardCapture(this, false);
		m_bInputFocus = false;
		m_iKeyPressAndHold = 0;
		return true;
		break;
	case DIK_BACKSPACE:
		if(m_iCursorPos > 0)
		{
			m_iCursorPos--;
			m_sEdit.erase(&m_sEdit[m_iCursorPos]);
		}
		str_updated = true; 
		break;
	case DIK_DELETE:
	case DIKEYBOARD_DELETE:
		if(m_iCursorPos < m_sEdit.size()-1)
		{
			m_sEdit.erase(&m_sEdit[m_iCursorPos]);
		}
		str_updated = true; 
		break;
	case DIK_SPACE:
		AddChar(' ');
		str_updated = true; 
		break;

	default:
		it = gs_DIK2CHR.find(dik);
			
		//������ ������� � ������ 
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

	return false;
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

	return false;
}



void CUIEditBox::AddChar(char c)
{
	char buf[2];
	buf[0]=c;
	buf[1]=0;
	
	
	int text_length;
	text_length = int(GetFont()->SizeOf(&m_sEdit.front()) + 
					GetFont()->SizeOf(buf));
	
	//������ ������ ������ �����
	if(text_length>GetWidth()) return;
	
	m_sEdit.insert(&m_sEdit[m_iCursorPos], c);
	m_iCursorPos++;

	/*if(!m_sEdit.empty()) m_sEdit.pop_back();
	m_sEdit.push_back(c);
	m_sEdit.push_back(0);*/
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


	

		//���������� ������
		RECT rect = GetAbsoluteRect();
		float outX, outY;

		STRING buf_str;
		buf_str.assign(&m_sEdit.front(), 
					   &m_sEdit[m_iCursorPos]);
		buf_str.push_back(0);

		outX = GetFont()->SizeOf(&buf_str.front());
		outY = 0;

		GetFont()->SetColor(0xAAFFFF00);
	    GetFont()->Out((float)rect.left+outX, 
					   (float)rect.top+outY,  "|");

	}
		
	m_str = &m_sEdit[0];
	CUIStatic::Update();
}