// UIString.h: объявление строки и функций работы с ней
//
//////////////////////////////////////////////////////////////////////

#pragma once

DEFINE_VECTOR	(char, STRING, STRING_IT);

class CUIString
{
public:
	CUIString();
	~CUIString();

	void SetText(LPSTR str);
	void AppendText(LPSTR str);
	
	char* GetBuf();

	operator char* ();
	operator const char* ();

	STRING m_str;
};



