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
	
	char* GetBuf();

	operator char* ();
	operator const char* ();

	STRING m_str;
};
