// UIString.h: ���������� ������ � ������� ������ � ���
//
//////////////////////////////////////////////////////////////////////

#pragma once

DEFINE_VECTOR	(char, STRING, STRING_IT);

class CUIString
{
public:
	CUIString();
	virtual ~CUIString();

	void SetText(LPCSTR str);
	void AppendText(LPCSTR str);
	
	LPCSTR GetBuf();
	operator LPCSTR ();

	STRING m_str;
};
