#include "stdafx.h"
#include "xrLauncher.h"
#include "xrLauncherControl.h"

#include "xrLauncher_main_frm.h"



int RunXRLauncher(int initial_state)
{

//	xrLauncher::xrLauncherControl* frm = new xrLauncher::xrLauncherControl();
/*
	CConsole* con = ::Console;
	con->Execute("snd_volume_eff 0.3");
	float fval,fmin,fmax;
	con->GetFloat("snd_volume_eff",fval,fmin,fmax);
	
*/
	xrLauncher::xrLauncher_main_frm* frm = new xrLauncher::xrLauncher_main_frm();
//	frm->Init(initial_state);
	return frm->_Show(initial_state);
}

