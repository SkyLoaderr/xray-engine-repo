#include "stdafx.h"
#include "xrLauncher.h"
#include "xrLauncherControl.h"

#include "xrLauncher_main_frm.h"


int __cdecl RunXRLauncher(int initial_state)
{
	xrLauncher::xrLauncher_main_frm* frm = new xrLauncher::xrLauncher_main_frm();
	return frm->_Show(initial_state);
}
