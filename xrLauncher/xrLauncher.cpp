#include "stdafx.h"
#include "xrLauncher.h"
#include "xrLauncherControl.h"

#include "xrLauncher_main_frm.h"
#include "xrLauncher_benchmark_res_frm.h"


int __cdecl RunXRLauncher(int initial_state)
{
	if(initial_state == 2){
		xrLauncher::xrLauncher_benchmark_res_frm * frm = new xrLauncher::xrLauncher_benchmark_res_frm();
		frm->_Show(0);

		return 0;
	}

	xrLauncher::xrLauncher_main_frm* frm = new xrLauncher::xrLauncher_main_frm();
	return frm->_Show(initial_state);
}
