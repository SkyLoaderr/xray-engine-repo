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

LPCSTR		log_file_name				= "vo.log";
const char	SHAPE_CHARACTER				= '*';
const int	CConsoleUI::g_display_width	= 79;
LPCSTR		header[]					= {
	"",
	"Board Game Engine v0.001",
	"Copyleft(C) 2000-2004 Dmitriy Iassenev",
};

CConsoleUI::CConsoleUI		()
{
	m_log			= fopen(log_file_name,"at");
	m_use_stdout	= true;
	m_use_log		= true;
	if (!m_log) {
		fprintf		(stderr,"Cannot open file %s!\n",log_file_name);
		exit		(1);
	}
	show_header		();
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

void CConsoleUI::show_header(const std::vector<LPCSTR> &strings)
{
	char								temp[g_display_width];
	std::vector<LPCSTR>::const_iterator	I = strings.begin();
	std::vector<LPCSTR>::const_iterator	E = strings.end();
	for ( ; I != E; ++I) {
		size_t				n = strlen(*I);
		bool				parity = !((g_display_width - n) & 1);
		size_t				count = ((g_display_width - 2 - n)/2 + (parity ? 0 : 1));
		Memory::mem_fill	(temp,' ',count*sizeof(char));
		temp[count]			= 0;
		strcat				(temp,*I);
		n					= strlen(temp);
		if (!parity)
			--count;
		Memory::mem_fill	(temp + n,' ',count*sizeof(char));
		temp[n+count]		= 0;
		log					("%c%s%c\n",SHAPE_CHARACTER,temp,SHAPE_CHARACTER);
	}
}

void CConsoleUI::show_header()
{
	char					compile[g_display_width];
	LPSTR					temp = process_compile();
	sprintf					(compile,"Compiled on %s %s",temp,__TIME__,SHAPE_CHARACTER);
	c_free					(temp);

	std::vector<LPCSTR>		strings;
	
	char					string[g_display_width - 1];
	Memory::mem_fill		(string,SHAPE_CHARACTER,(g_display_width - 2)*sizeof(char));
	string[g_display_width - 2] = 0;

	strings.push_back		(string);

	for (u32 i=0, n=sizeof(header)/sizeof(header[0]); i<n; ++i)
		strings.push_back	(header[i]);

	strings.push_back		(compile);
	strings.push_back		("");
	strings.push_back		(string);

	show_header				(strings);
}

int CConsoleUI::pure_log	(LPCSTR format, va_list list)
{
	int						result = 0;
	
	if (m_use_stdout)
		result				= vfprintf(stdout,format,list);
	
	if (m_use_log)
		result				= vfprintf(m_log,format,list);

#ifdef _DEBUG
	string256				temp;
	vsprintf				(temp,format,list);
	OutputDebugString		(temp);
#endif
	return					(result);
}

template <CConsoleUI::EMessageType type>
int __cdecl CConsoleUI::log	(LPCSTR format, va_list list)
{
	va_list					marker;
	va_start				(marker,format);

	switch (type) {
		case eMessageTypeCore : {
			break;
		}
		case eMessageTypeScript : {
			pure_log		("Script : ");
			break;
		}
		case eMessageTypeWarning : {
			pure_log		("Warning : ");
			break;
		}
		case eMessageTypeError : {
			pure_log		("Error : ");
			break;
		}
		default : NODEFAULT;
	}

	int						result = pure_log(format,list);

	switch (type) {
		case eMessageTypeCore : {
			break;
		}
		case eMessageTypeScript : {
			pure_log		("\n");
			break;
		}
		case eMessageTypeWarning : {
			break;
		}
		case eMessageTypeError : {
			break;
		}
		default : NODEFAULT;
	}

	return					(result);
}

int __cdecl	CConsoleUI::log	(LPCSTR format, ...)
{
	va_list					marker;
	va_start				(marker,format);
	int						result = log<eMessageTypeCore>		(format,marker);
	va_end					(marker);
	return					(result);
}

int __cdecl	CConsoleUI::script_log	(LPCSTR format, ...)
{
	va_list					marker;
	va_start				(marker,format);
	int						result = log<eMessageTypeScript>	(format,marker);
	va_end					(marker);
	return					(result);
}

int __cdecl	CConsoleUI::warning_log	(LPCSTR format, ...)
{
	va_list					marker;
	va_start				(marker,format);
	int						result = log<eMessageTypeWarning>	(format,marker);
	va_end					(marker);
	return					(result);
}

int __cdecl	CConsoleUI::error_log	(LPCSTR format, ...)
{
	va_list					marker;
	va_start				(marker,format);
	int						result = log<eMessageTypeError>		(format,marker);
	va_end					(marker);
	return					(result);
}

void CConsoleUI::execute	(char argc, char *argv[])
{
	string256				s;
	xr_strcpy				(s,"");
	for (char i=1; i<argc; ++i) {
		sprintf				(s,"dofile(\"%s\")\n",argv[i]);
		ui().log			(s);
		script().run_string	(s);
	}

	for (;;) {
		log					("bge>");
		xr_strcpy			(s,"");
		scanf				("%[^\n]s %c",s);
		m_use_stdout		= false;
		log					("%s\n",s);
		m_use_stdout		= true;
		script().run_string	(s);
		scanf				("%c",s);
	}
}

void CConsoleUI::flush		()
{
	fflush					(stdout);
	if (!m_log)
		return;
	fflush					(m_log);
}
