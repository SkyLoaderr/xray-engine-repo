#include "stdafx.h"
#pragma hdrstop

#include "Statistic.h"

//---------------------------------------------------------------------------
CStatistic::CStatistic()
{
    Reset();
    dwTotalFrame = 0;
}

void CStatistic::Reset()
{
    dwRenderPolyCount = 0;
    dwTotalLight = 0;
    dwLightInScene = 0;
}

#pragma package(smart_init)
