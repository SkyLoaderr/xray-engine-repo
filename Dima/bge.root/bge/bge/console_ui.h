////////////////////////////////////////////////////////////////////////////
//	Module 		: console_ui.h
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Console interface
////////////////////////////////////////////////////////////////////////////

#pragma once

class CConsoleUI {
public:
			int __cdecl log		(LPCSTR format, ...);
			void		execute	(char argc, char *argv[]);
};
