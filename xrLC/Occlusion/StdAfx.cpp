// StdAfx.cpp: implementation of the CStdAfx class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ENGINE_API void __cdecl    _VERIFY_dlg (const char *expr, char *filename, unsigned lineno );
ENGINE_API void __fastcall _verify     (const char *expr, char *file, int line) {
	char _tmp_buf[128];
	sprintf(_tmp_buf,"Assertion failed in file %s, line %d.\nExpression was '%s'",
		file, line, expr);
	Log(_tmp_buf);
	_VERIFY_dlg(expr, file, line);
	__asm { int 3 };
}

ENGINE_API vector <char *> _stack_unwind_list;
ENGINE_API void __fastcall _unroll(const char *s1, const char *s2) {
	char buf[256];
	sprintf(buf, "%s <=> %s",s1,s2);
    _stack_unwind_list.push_back(strdup(buf));
}

ENGINE_API void __fastcall _stack_display(void) {
	char m_stack[8192];
	
	m_stack[0]=0;
	Log("\nStack unrolling after last exception:");
	for (int i=0; i<_stack_unwind_list.size(); i++) {
		Log("* ",_stack_unwind_list[i]);
		strcat(m_stack,_stack_unwind_list[i]);
		strcat(m_stack,"\n");
	}
	MessageBox(NULL,m_stack,"Stack unrolling",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
}

//--------------------------------------------------------------------

#define BOXINTRO    "Assertion failed!"
#define PROGINTRO   "Program: "
#define FILEINTRO   "File: "
#define LINEINTRO   "Line: "
#define EXPRINTRO   "Expression: "
#define HELPINTRO   "Press Retry to debug the application"

static char * dotdotdot  = "...";
static char * newline    = "\n";
static char * dblnewline = "\n\n";

#define DOTDOTDOTSZ             3
#define NEWLINESZ               1
#define DBLNEWLINESZ    2

#define MAXLINELEN  60 /* max length for line in message box */
#define ASSERTBUFSZ (MAXLINELEN * 9) /* 9 lines in message box */

ENGINE_API void __cdecl _VERIFY_dlg (
									 const char *expr,
									 char *filename,
									 unsigned lineno
									 )
{
	int nCode;
	char * pch;
	char VERIFYbuf[ASSERTBUFSZ];
	char progname[MAX_PATH];
	
	/*
	* Line 1: box intro line
	*/
	strcpy( VERIFYbuf, BOXINTRO );
	strcat( VERIFYbuf, dblnewline );
	
	/*
	* Line 2: program line
	*/
	strcat( VERIFYbuf, PROGINTRO );
	
	if ( !GetModuleFileName( NULL, progname, MAX_PATH ))
		strcpy( progname, "<program name unknown>");
	
	pch = (char *)progname;
	
	/* sizeof(PROGINTRO) includes the NULL terminator */
	if ( sizeof(PROGINTRO) + strlen(progname) + NEWLINESZ > MAXLINELEN )
	{
		pch += (sizeof(PROGINTRO) + strlen(progname) + NEWLINESZ) - MAXLINELEN;
		strncpy( pch, dotdotdot, DOTDOTDOTSZ );
	}
	
	strcat( VERIFYbuf, pch );
	strcat( VERIFYbuf, newline );
	
	/*
	* Line 3: file line
	*/
	strcat( VERIFYbuf, FILEINTRO );
	
	/* sizeof(FILEINTRO) includes the NULL terminator */
	if ( sizeof(FILEINTRO) + strlen(filename) + NEWLINESZ >
		MAXLINELEN )
	{
		/* too long. use only the first part of the filename string */
		strncat( VERIFYbuf, filename, MAXLINELEN - sizeof(FILEINTRO)
			- DOTDOTDOTSZ - NEWLINESZ );
		/* append trailing "..." */
		strcat( VERIFYbuf, dotdotdot );
	}
	else
		/* plenty of room on the line, just append the filename */
		strcat( VERIFYbuf, filename );
	
	strcat( VERIFYbuf, newline );
	
	/*
	* Line 4: line line
	*/
	strcat( VERIFYbuf, LINEINTRO );
	_itoa( lineno, VERIFYbuf + strlen(VERIFYbuf), 10 );
	strcat( VERIFYbuf, dblnewline );
	
	/*
	* Line 5: message line
	*/
	strcat( VERIFYbuf, EXPRINTRO );
	
	/* sizeof(HELPINTRO) includes the NULL terminator */
	
	if (    strlen(VERIFYbuf) +
		strlen(expr) +
		2*DBLNEWLINESZ +
		//              sizeof(INFOINTRO)-1 +
		sizeof(HELPINTRO) > ASSERTBUFSZ )
	{
		strncat( VERIFYbuf, expr,
			ASSERTBUFSZ -
			(strlen(VERIFYbuf) +
			DOTDOTDOTSZ +
			2*DBLNEWLINESZ +
			//                      sizeof(INFOINTRO)-1 +
			sizeof(HELPINTRO)) );
		strcat( VERIFYbuf, dotdotdot );
	}
	else
		strcat( VERIFYbuf, expr );
	
	strcat( VERIFYbuf, dblnewline );
	
	/*
	* Line 6, 7: info line
	*/
	
	//          strcat(VERIFYbuf, INFOINTRO);
	//          strcat( VERIFYbuf, dblnewline );
	
	/*
	* Line 8: help line
	*/
	strcat(VERIFYbuf, HELPINTRO);
	strcat(VERIFYbuf, "\nContinue execution?");
	
	/*
	* Write out via MessageBox
	*/
	
	nCode = MessageBox(NULL, VERIFYbuf,
		"Engine internal error",
		MB_YESNO|MB_ICONSTOP|MB_SETFOREGROUND|MB_TASKMODAL);
	
	if (nCode == IDNO)
	{
		THROW;
	}
	
	if (nCode == IDYES)
	{
		return;
	}
}
