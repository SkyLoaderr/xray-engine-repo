//---------------------------------------------------------------------------

#ifndef StatisticH
#define StatisticH
//---------------------------------------------------------------------------
class CStatistic{
public:
    DWORD dwRenderPolyCount;
    DWORD dwTotalLight;
    DWORD dwLightInScene;
    DWORD dwTotalFrame;

    CStatistic();
    void Reset();
};
#endif
