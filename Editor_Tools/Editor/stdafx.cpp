#include "stdafx.h"
#pragma hdrstop

bool g_ErrorMode=false;

LPCSTR InterpretError(HRESULT hr){
	static char errStr[1024];
    D3DXGetErrorString(hr, errStr, 1024 );
    return errStr;
}

void __fastcall _verify(const char *expr, char *file, int line) {
	// Log Description
	int res = ELog.DlgMsg(mtError,TMsgDlgButtons()<<mbOK<<mbAbort<<mbIgnore,"Assertion failed in file %s, line %d.\nExpression was\n'%s'\n\nClick on OK to debug the program.",file, line, expr);
	switch (res) {
	case mrAbort:
        TerminateProcess(GetCurrentProcess(),-1);
		break;
	case mrOk:
    	DebugBreak();
		break;
	case mrIgnore:
		break;
	}
}

// Initialized on startup
ENGINE_API Fbox box_identity;

// startup point
class StaticInit{
public:
	StaticInit(){
        box_identity.set(-0.5f,-0.5f,-0.5f,0.5f,0.5f,0.5f);
    }
};
StaticInit SI;

