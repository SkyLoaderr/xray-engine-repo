////////////////////////////////////////////////////////////////////////////
//	Module 		: console_ui.cpp
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Console interface
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "console_ui.h"
#include "script.h"

LPCSTR		log_file_name		= "vo.log";
const char	SHAPE_CHARACTER		= '*';
const int	display_width		= 79;
LPCSTR		header[]			= {
	"",
	"Board Game Engine v0.001",
	"Copyleft(C) 2000-2004 Dmitriy Iassenev",
};

CConsoleUI::CConsoleUI		()
{
	m_log		= fopen(log_file_name,"at");
	if (!m_log) {
		fprintf	(stderr,"Cannot open file %s!\n",log_file_name);
		exit	(1);
	}
	show_header	();
}

CConsoleUI::~CConsoleUI		()
{
	if (!m_log) {
		fprintf	(stderr,"Cannot close file %s!\n",log_file_name);
		exit	(2);
	}
	fclose		(m_log);
}

LPSTR CConsoleUI::process_compile	()
{
	LPCSTR		date = __DATE__;
	LPSTR		result = strdup(date);
	bool		brace = false;
	for (int i=0, n=(int)strlen(date); i<n; ++i)
		if ((date[i] != ' ') || !brace) {
			if (date[i] == ' ') 
				brace = true;
			else 
				brace = false;
        
			result[i] = date[i];
		}
		else 
			result[i] = '0';
	return		(result);
}

void CConsoleUI::show_header(const vector<LPCSTR> &strings)
{
	char							temp[display_width];
	vector<LPCSTR>::const_iterator	I = strings.begin();
	vector<LPCSTR>::const_iterator	E = strings.end();
	for ( ; I != E; ++I) {
		size_t				n = strlen(*I);
		bool				parity = !((display_width - n) & 1);
		size_t				count = ((display_width - 2 - n)/2 + (parity ? 0 : 1));
		memset				(temp,' ',count*sizeof(char));
		temp[count]			= 0;
		strcat				(temp,*I);
		n					= strlen(temp);
		if (!parity)
			--count;
		memset				(temp + n,' ',count*sizeof(char));
		temp[n+count]		= 0;
		log					("%c%s%c\n",SHAPE_CHARACTER,temp,SHAPE_CHARACTER);
	}
}

void CConsoleUI::show_header()
{
	char					compile[display_width];
	LPSTR					temp = process_compile();
	sprintf					(compile,"Compiled on %s %s",temp,__TIME__,SHAPE_CHARACTER);
	c_free					(temp);

	vector<LPCSTR>			strings;
	
	char					string[display_width-1];
	memset					(string,SHAPE_CHARACTER,(display_width - 2)*sizeof(char));
	string[display_width - 2] = 0;

	strings.push_back		(string);

	for (u32 i=0, n=sizeof(header)/sizeof(header[0]); i<n; ++i)
		strings.push_back	(header[i]);

	strings.push_back		(compile);
	strings.push_back		("");
	strings.push_back		(string);

	show_header				(strings);
}

int __cdecl CConsoleUI::log	(LPCSTR format, ...)
{
	va_list		marker;

	va_start	(marker,format);

	int			result = vfprintf(stdout,format,marker);
	vfprintf	(m_log,format,marker);

	va_end		(marker);

	return		(result);
}

void CConsoleUI::execute	(char argc, char *argv[])
{
	char s[128];
	for (;;) {
		log					("bge> ");
		scanf				("%[^\n]s",s);
		script().run_string	(s);
		scanf				("%[^\r]s",s);
	}
}
