// UIString.h: объявление строки и функций работы с ней
//
//////////////////////////////////////////////////////////////////////

#pragma once

DEFINE_VECTOR	(char, STRING, STRING_IT);

class CUIString
{
public:
	CUIString();
#pragma todo("Dima to Yura : check if you need here a non-virtual destructor!")
	~CUIString();

	void SetText(LPSTR str);
	void AppendText(LPSTR str);
	
	char* GetBuf();

	operator char* ();
	operator const char* ();

	STRING m_str;
};



