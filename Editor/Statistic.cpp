//---------------------------------------------------------------------------
#include "PCH.h"
#pragma hdrstop

#include "Statistic.h"

//---------------------------------------------------------------------------
CStatistic::CStatistic()
{
    Reset();
    lTotalFrame = 0;
}

void CStatistic::Reset()
{
    lRenderPolyCount = 0;
    lTotalLight = 0;
    lLightInScene = 0;
}

#pragma package(smart_init)
