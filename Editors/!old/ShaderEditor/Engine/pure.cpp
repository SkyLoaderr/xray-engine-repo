#include "stdafx.h"
#pragma hdrstop

#include "pure.h"

ENGINE_API int	__cdecl	_REG_Compare(const void *e1, const void *e2)
{
	_REG_INFO *p1 = (_REG_INFO *)e1;
	_REG_INFO *p2 = (_REG_INFO *)e2;
	return (p2->Prio - p1->Prio);
}

#define DECLARE_RP(name) void __fastcall rp_##name(void *p) { ((pure##name *)p)->On##name(); }

DECLARE_RP(Frame);
DECLARE_RP(Render);
DECLARE_RP(AppActivate);
DECLARE_RP(AppDeactivate);
DECLARE_RP(AppCycleStart);
DECLARE_RP(AppCycleEnd);
