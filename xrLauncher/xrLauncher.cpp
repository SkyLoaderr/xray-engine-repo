#include "stdafx.h"
#include "xrLauncher.h"
#include "xrLauncherControl.h"


//#include "../xrCore/_types.h"
//#include "../xrCore/xrCore.h"
#include "../xray/xr_ioc_wrapper.h"

using namespace xrLauncher;


int RunXRLauncher()
{
	CConsoleWrapper con;// = new CConsoleWrapper();
//	con.Execute("quit");

	xrLauncher::xrLauncherControl* frm = new xrLauncher::xrLauncherControl();
	frm->Init();
	frm->ShowDialog();
	return 1;
}

