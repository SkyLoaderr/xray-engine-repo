#include "stdafx.h"
//#pragma hdrstop

bool g_ErrorMode=false;
void __fastcall _verify(const char *expr, char *file, int line) {
	// Log Description
/*	int res = Log->DlgMsg(mtError,TMsgDlgButtons()<<mbOK<<mbAbort<<mbIgnore,"Assertion failed in file %s, line %d.\nExpression was '%s'\n\nClick on OK to debug the program.",file, line, expr);
	switch (res) {
	case mrAbort:
        TerminateProcess(GetCurrentProcess(),-1);
//    	abort();
//      Application->Terminate();
		break;
	case mrOk:
    	DebugBreak();
		break;
	case mrIgnore:
		break;
	}
*/
}
/*
static float _identity[4][4] = {
{	1,	0,	0,	0 },
{	0,	1,	0,	0 },
{	0,	0,	1,	0 },
{	0,	0,	0,	1 } };

// Initialized on startup
ENGINE_API Fmatrix precalc_identity;

class StaticInit{
public:
	StaticInit(){
		CopyMemory(&precalc_identity,&_identity,sizeof(Fmatrix  ));
    }
};
StaticInit SI;

*/