////////////////////////////////////////////////////////////////////////////
//	Module 		: console_ui.h
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Console interface
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CConsoleUI {
public:
	enum EMessageType {
		eMessageTypeCore	= u32(0),
		eMessageTypeScript,
		eMessageTypeWarning,
		eMessageTypeError,
		eMessageTypeDummy	= u32(-1),
	};

private:
	FILE				*m_log;
	static const int	g_display_width;
	bool				m_use_stdout;
	bool				m_use_log;

protected:
			LPSTR		process_compile	();
			void		show_header		(const xr_vector<LPCSTR> &strings);
	virtual	void		show_header		();
			int			pure_log		(LPCSTR format, va_list list = va_list());

	template <EMessageType type>
			int __cdecl log				(LPCSTR format, va_list list);

public:
						CConsoleUI		();
	virtual				~CConsoleUI		();
			int __cdecl	log				(LPCSTR format, ...);
			int __cdecl	script_log		(LPCSTR format, ...);
			int __cdecl	warning_log		(LPCSTR format, ...);
			int __cdecl	error_log		(LPCSTR format, ...);
			void		execute			(char argc, char *argv[]);
			void		flush			();
	IC		int			display_width	() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CConsoleUI)
#undef script_type_list
#define script_type_list save_type_list(CConsoleUI)

#include "console_ui_inline.h"