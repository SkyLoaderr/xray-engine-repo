#include "stdafx.h"
#include "BugSlayerUtil.h"
#include <stdio.h>

#define MAX_STACK_TRACE	100

char g_stackTrace[MAX_STACK_TRACE][4096];
int g_stackTraceCount = 0;

void BuildStackTrace(struct _EXCEPTION_POINTERS *g_BlackBoxUIExPtrs)
{
	printf				("building stack trace\n");

	FillMemory			(g_stackTrace[0],MAX_STACK_TRACE*256, 0 );

	const TCHAR* traceDump = 
		GetFirstStackTraceString( GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE,
									g_BlackBoxUIExPtrs );
	g_stackTraceCount = 0;

	int incr = 85;
	while ( NULL != traceDump ) {
		lstrcpy( g_stackTrace[g_stackTraceCount], traceDump );		
	
		g_stackTraceCount++;

		incr += 2;
		traceDump = GetNextStackTraceString( GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE,
			g_BlackBoxUIExPtrs );		
	}
}
