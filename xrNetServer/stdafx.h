// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Third generation by Oles.

#ifndef stdafxH
#define stdafxH

#pragma once

#include <xrCore.h>

#pragma warning(disable:4995)
#include <dplay8.h>
#pragma warning(default:4995)

#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }
#define _SHOW_REF(msg, x)   { if(x) { x->AddRef(); Log(msg,u32(x->Release()));}}

IC u32 TimeGlobal(CTimer* timer)
{
	u64	qTime	= timer->GetElapsed_clk();
	return		u32((qTime*u64(1000))/CPU::cycles_per_second);
}
#define TimerAsync TimeGlobal

#endif //stdafxH
