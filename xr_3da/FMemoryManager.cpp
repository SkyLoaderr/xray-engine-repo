#include "stdafx.h"
#include "xr_input.h"
//#define TESTMEM
//#define WRITESYNC

#ifdef DEBUG
FILE *			memLOG	= NULL;
_CrtMemState	mmgrState;
#endif

/* ALLOCATION HOOK FUNCTION   -----------------------------------
   An allocation hook function can have many, many different
   uses. This one simply logs each allocation operation in a file.
*/
#ifdef DEBUG
int __cdecl MyAllocHook(
	int		nAllocType,
	void*	pvData,
	size_t  nSize,
	int     nBlockUse,
	long    lRequest,
	const	unsigned char *szFileName,
	int     nLine   )
{
   char *operation[] = { "?", "ALLOC", "REALLOC", "FREE"				};
   char *blockType[] = { "FREE", "NORMAL", "CRT", "IGNORE", "CLIENT"	};

   if (nBlockUse==_CRT_BLOCK) return( TRUE );

//	if (pInput && pInput->iGetAsyncKeyState(DIK_LSHIFT)) __asm int 3;

   VERIFY( ( nAllocType > 0 ) && ( nAllocType < 4 ) );
   VERIFY( ( nBlockUse >= 0 ) && ( nBlockUse < 5 ) );
   if (nAllocType==3)  // FREE
	   nSize = _msize(pvData);
   if (memLOG) {
		fprintf( memLOG,
			"%7s: %6d b | #%5ld | %s(%d)\n",
			operation[nAllocType],
			nSize,
			lRequest,
			szFileName,
			nLine
		);
#ifdef WRITESYNC
		fflush(memLOG);
#endif
   }
//	if (memLOG)	fclose(memLOG);
//   if ( pvData != NULL )
//    fprintf( logFile, " at %X", pvData );
   return( TRUE );         // Allow the memory operation to proceed
}
/* REPORT HOOK FUNCTION   --------------------
   Again, report hook functions can serve a very wide variety of purposes.
   This one logs error and VERIFYion failure debug reports in the
   log file, along with 'Damage' reports about overwritten memory.
   By setting the retVal parameter to zero, we are instructing _CrtDbgReport
   to return zero, which causes execution to continue. If we want the function
   to start the debugger, we should have _CrtDbgReport return one.*/
int __cdecl MyReportHook(
	int   nRptType,
	char *szMsg,
	int  *retVal
	)
{
	char *RptTypes[] = { "!WARNING", "!ERROR", "!ASSERT" };
//	memLOG = fopen( "memory.log", "at" );
	if (memLOG) {
		fprintf( memLOG, "%-8s: %s\n", RptTypes[nRptType], szMsg );
#ifdef WRITESYNC
		fflush(memLOG);
#endif
	}
//	if (memLOG)	fclose(memLOG);
	*retVal = 0;
	return( FALSE );         // Allow the report to be made as usual
}
#endif

void mmgrInitialize(int dbgLevel)
{
#ifdef DEBUG

	int CRTflag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ) | _CRTDBG_ALLOC_MEM_DF;
	switch (dbgLevel) {
	case 3:	// check every memory operation (VERY SLOW!!!)
		CRTflag |= _CRTDBG_CHECK_ALWAYS_DF;
	case 2: // verify crt blocks
//		CRTflag |= _CRTDBG_CHECK_CRT_DF;
	case 1: // only log & leaks check on exit
		memLOG = fopen( "memory.log", "wt" );
		if (memLOG){
			long Time;	time(&Time);
			fprintf(memLOG, "%s\n", ctime(&Time));
		}
		_CrtSetDbgFlag( CRTflag );
		_CrtSetAllocHook( MyAllocHook );
		_CrtSetReportHook( MyReportHook );
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		break;
	case 0:
	default:
		break;
	}
	_CrtMemCheckpoint(&mmgrState);
#endif
}

void mmgrDone(void)
{
#ifdef DEBUG
#ifdef TESTMEM
	void *test;
	mmgrMessage("**** Generating memory leak");
	test = malloc(100);
	test = malloc(50);
	mmgrMessage("**** Generating memory damage");
	free(test);
	ZeroMemory(test,50);
#endif
	mmgrMessage("**** Checking memory consistency...");
	_CrtCheckMemory();
	mmgrMessage("**** Checking memory leaks...");
	_CrtMemDumpAllObjectsSince(&mmgrState);
//	_CrtDumpMemoryLeaks();
	if (memLOG)	fclose(memLOG);
#endif
}

void mmgrMessage(const char *logMSG, const char *dop)
{
#ifdef DEBUG
	if (memLOG) {
		fprintf(memLOG,"%s %s\n",logMSG,dop);
#ifdef WRITESYNC
		fflush(memLOG);
#endif
	}
#endif
}

char * __fastcall _STRDUP(const char * string)
{
	VERIFY(string);
	int		len		= strlen(string)+1;
	char *	memory	= (char *) malloc( len );
	VERIFY(memory);
	CopyMemory(memory,string,len);
	return memory;
}
