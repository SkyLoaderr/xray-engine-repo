////////////////////////////////////////////////////////////////////////////
//	Module 		: console_ui.h
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Console interface
////////////////////////////////////////////////////////////////////////////

#pragma once

class CConsoleUI {
private:
	FILE				*m_log;

protected:
			LPSTR		process_compile	();
			void		show_header		(const vector<LPCSTR> &strings);
	virtual	void		show_header		();

public:
						CConsoleUI		();
	virtual				~CConsoleUI		();
			int __cdecl log				(LPCSTR format, ...);
			void		execute			(char argc, char *argv[]);
};
